import { RawData } from "ws";
import { NTConnection } from "./NTConnection";
import { encode, decode, decodeMulti, decodeArrayStream } from '@msgpack/msgpack' ;
import { NTValue } from "./NTValue";
import { NTBinaryMessage } from "./NTBinaryMessage";
import { NTMessageLogger, NTMessageType } from "./NTMessageLogger";
import { NTCurrentTime } from "./NTTime" ;
import { EventEmitter } from 'node:events' ;
import { NTSubscribeOptions } from "./NTSubscribe";
import { NTTopicTree } from "./NTTopicTree";
import { NTTopic } from "./NTTopic";

type ValueCallback = (msg: NTBinaryMessage) => void ;
export class NTClient extends EventEmitter {
    private connection_ : NTConnection ;
    private lastSent_ : number ;
    private logger_ : NTMessageLogger ;
    private pongTimeMs_ : number ;
    private rtt2us_ : number ;
    private time_offset_ : number | undefined ;
    private valuecb_ : ValueCallback | null ;
    private subid_ : number ;
    private topics_ : NTTopicTree ;

    private static kMinPeriodMs : number = 5 ;

    public constructor(logger: NTMessageLogger, addr: string) {
        super() ;
        this.logger_ = logger ;
        this.lastSent_ = 0 ;
        this.connection_ = new NTConnection(addr) ;
        this.pongTimeMs_ = 0 ;
        this.rtt2us_ = Number.MAX_VALUE ;
        this.time_offset_ = undefined ;
        this.valuecb_ = null ;
        this.subid_ = 1 ;
        this.topics_ = new NTTopicTree(logger) ;

        this.connection_.on('connected', () => { this.connected() ; });
        this.connection_.on('disconnected', (code: number, reason: Buffer) => { this.disconnected(code, reason) ; });
        this.connection_.on('error', (err: Error) => { this.error(err) ; });
        this.connection_.on('binary', (data: RawData) => { this.binaryMessage(data)}) ;
        this.connection_.on('json', (obj: Object) => { this.jsonMessage(obj)}) ;

        this.connection_.connect() ;
    }

    public isConnected() : boolean { 
        return this.connection_ !== null && this.connection_.isConnected() ;
    }

    public setValueCallback(cb: ValueCallback) {
        this.valuecb_ = cb ;
    }

    public close() : void {
        //
        // TODO: what do we do if we are connecting
        //
        if (this.connection_ !== null && this.connection_.isConnected()) {
            this.connection_.disconnect() ;
        }
    }

    private connected() : void {
        this.logger_.log(NTMessageType.Info, "connected to NT server");
        this.sendRTTPing() ;
    }
    
    private sendRTTPing() {
        let v = new NTValue() ;
        v.initInteger(NTCurrentTime());
        this.wireEncodeBinary(-1, 0, v) ;
    }

    private wireEncodeBinary(id: number, time: number, value: NTValue) {
        let packet : any[] = [] ;

        packet.push(id) ;
        packet.push(time);
        value.pack(packet);

        const data = encode(packet);
        this.connection_.send(data) ;
    }

    private disconnected(code: number, reason: Buffer) : void {
        this.logger_.log(NTMessageType.Error, "server disconnected: code %d, reason '%s'", code, reason.toString());
        this.connection_.connect();
    }    

    private error(err: Error) {
        this.logger_.log(NTMessageType.Error, "error in winsock connection '%s'", err.message);
        if (this.connection_.isConnected())
            this.connection_.disconnect() ;
        this.connection_.connect();
    }

    private binaryMessage(data: RawData) {
        let buf = data as Buffer ;

        let objg = decodeMulti(buf) ;
        while (true) {
            let obj = objg.next() ;
            if (obj.done === true)
                break ;
            let msg: NTBinaryMessage = new NTBinaryMessage(obj.value as any[]) ;

            if (msg.id_ === -1) {
                //
                // This is a RTT ping response
                //
                if (msg.value_.isInteger()) {
                    this.logger_.log(NTMessageType.Debug, "processing server RTT response");
                    this.pongTimeMs_ = NTCurrentTime() ;
                    let now = NTCurrentTime();
                    let rtt2 = (now - msg.value_.asInteger()) / 2 ;
                    if (rtt2 < this.rtt2us_) {
                        this.rtt2us_ = rtt2 ;
                        this.time_offset_ = msg.timestamp_ + rtt2 - now ;
                        this.logger_.log(NTMessageType.Debug, "time offset: %d", this.time_offset_);
                    }
                    this.emit('connected');
                }
                else {
                    this.logger_.log(NTMessageType.Warning, "received RTT packet with non-integer payload - type '%s'", msg.value_.typeString());
                }
            }
            else {
                if (this.valuecb_ !== null) {
                    this.valuecb_(msg);
                }
            }
        }
    }

    private checkvalid(obj: Object, name: string, type: string) : boolean {
        if (obj[name] === undefined) {
            this.logger_.log(NTMessageType.Debug, "received JSON message without '" + name + "' field in 'params' - ignoring");             
            return false ;
        }

        if (typeof obj[name] !== type) {
            this.logger_.log(NTMessageType.Debug, "received JSON message with wrong type for field '" + name + 
                            ", expected '" + type + "', got '" + typeof obj[name] + "'") ;
            return false ;
        }

        return true ;
    }

    private handleOneJSONMessage(obj: Object) {
        const method = obj['method'] ;
        if (method === undefined) {
            this.logger_.log(NTMessageType.Debug, "received JSON message without 'method' field - ignoring");
            return ;
        }
        const params = obj['params'] ;
        if (params === undefined) {
            this.logger_.log(NTMessageType.Debug, "received JSON message without 'params' field - ignoring");
            return ;                
        }        

        if (method === 'announce') {
            if (!this.checkvalid(params, 'name', 'string'))
                return ;

            if (!this.checkvalid(params, 'id', 'number'))
                return ;

            let topic: NTTopic = this.topics_.add(params['name'], params['id']) ;

            if (params['properties'] !== undefined) {
                topic.addProps(params['properties']) ;
            }

            this.emit('announce', topic);
        } else if (method === 'unannounce') {
        } else if (method === 'properties') {
        } else {
            this.logger_.log(NTMessageType.Debug, "received JSON message with invalid 'method' field value '" + method + "' - ignoring") ;
        }
    }

    private jsonMessage(msg: Object) {
        let obj = JSON.parse(msg.toString()) ;
        let objs: Object[] = obj as Object[] ;
        objs.forEach((one) => { 
            this.handleOneJSONMessage(one) ;
        }) ;
    }

    public subscribe(topics: string[], options? : NTSubscribeOptions) : number {
        let ret = this.subid_++ ;

        if (options === undefined) {
            options = {} ;
        }

        let obj = 
        {
            "method": "subscribe",
            "params": {
                "options": options,
                "topics": topics,
                "subuid": ret
            }
        } ;

        let objs = [ obj] ;

        const str = JSON.stringify(objs) ;
        this.connection_.sendJson(str) ;

        return ret ;
    }
}
