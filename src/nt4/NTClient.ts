import { RawData } from "ws";
import { NTConnection } from "./NTConnection";
import { encode, decode, decodeMulti, decodeArrayStream } from '@msgpack/msgpack' ;
import { NTType, NTValue } from "./NTValue";
import { NTBinaryMessage } from "./NTBinaryMessage";
import { NTMessageLogger, NTMessageType } from "./NTMessageLogger";
import { NTCurrentTime } from "./NTTime" ;
import { EventEmitter } from 'node:events' ;
import { NTPublishOptions, NTSubscribeOptions } from "./NTOptions";
import { NTTopicTree } from "./NTTopicTree";
import { NTLooseObject, NTTopic } from "./NTTopic";

type ValueCallback = (msg: NTBinaryMessage) => void ;
export class NTClient extends EventEmitter {
    private rtt_interval_ : number ;
    private connection_ : NTConnection ;
    private logger_ : NTMessageLogger ;
    private rtt2us_ : number ;
    private time_offset_ : number | undefined ;
    private subid_ : number ;
    private pubid_ : number ;
    private topics_ : NTTopicTree ;
    private connected_ : boolean ;

    private static kMinPeriodMs : number = 5 ;

    public constructor(id: string, host: string, port: number) {
        super() ;

        let addr: string = "ws://" + host + ":" + port + "/nt/" + id ;

        this.logger_ = new NTMessageLogger();
        this.connection_ = new NTConnection(addr) ;
        this.rtt2us_ = Number.MAX_VALUE ;
        this.time_offset_ = undefined ;
        this.subid_ = 1 ;
        this.pubid_ = 1 ;
        this.topics_ = new NTTopicTree(this.logger_) ;
        this.connected_ = false ;
        this.rtt_interval_ = 2000 ;

        this.connection_.on('connected', () => { this.connected() ; });
        this.connection_.on('disconnected', (code: number, reason: Buffer) => { this.disconnected(code, reason) ; });
        this.connection_.on('error', (err: Error) => { this.error(err) ; });
        this.connection_.on('binary', (data: RawData) => { this.binaryMessage(data)}) ;
        this.connection_.on('json', (obj: Object) => { this.jsonMessage(obj)}) ;

        this.connection_.connect() ;
    }

    public getLogger() : NTMessageLogger {
        return this.logger_ ;
    }

    public isConnected() : boolean { 
        return this.connection_ !== null && this.connection_.isConnected() ;
    }

    public close() : void {
        //
        // TODO: what do we do if we are connecting
        //
        if (this.connection_ !== null && this.connection_.isConnected()) {
            this.connection_.disconnect() ;
        }
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

        const str = JSON.stringify([obj]) ;
        this.connection_.sendJson(str) ;

        return ret ;
    }

    public publish(name: string, type: NTType, options?: NTPublishOptions) : number {
        let ret = this.pubid_++ ;
        
        let obj = 
        {
            "method": "publish",
            "params": {
                "name" : name,
                "pubuid": ret,
                "type" : NTValue.typeString(type),
                "properties" : options
            }
        } ;

        const str = JSON.stringify([obj]) ;
        this.connection_.sendJson(str) ;

        return ret ;
    }

    public release(id: number) : void {
        let obj = 
        {
            "method": "unpublish",
            "params": {
                "pubuid": id
            }
        } ;

        const str = JSON.stringify([obj]) ;
        this.connection_.sendJson(str) ;
    }

    public setProperties(topic: NTTopic, props: NTLooseObject) {
        let obj = 
        {
            "method": "setproperties",
            "params": {
                "name": topic.name_,
                "update" : props
            }
        } ;

        const str = JSON.stringify([obj]) ;
        this.connection_.sendJson(str) ;        
    }

    public setValueByName(name: string, value: NTValue) {
        let topic: NTTopic | null = this.topics_.getTopicByName(name) ;
        if (topic !== null && this.time_offset_ !== undefined) {
            this.wireEncodeBinary(topic.pubuid_, NTCurrentTime() + this.time_offset_, value) ;
        }
    } 

    public setValue(topic: NTTopic, value: NTValue) {
        if (this.time_offset_ !== undefined) {
            this.wireEncodeBinary(topic.pubuid_, NTCurrentTime() + this.time_offset_, value) ;
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
        this.connected_ = false ;
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
                    let now = NTCurrentTime();
                    let rtt2 = (now - msg.value_.asInteger()) / 2 ;
                    if (rtt2 < this.rtt2us_) {
                        this.rtt2us_ = rtt2 ;
                        this.time_offset_ = msg.timestamp_ + rtt2 - now ;
                        this.logger_.log(NTMessageType.Debug, "time offset: %d", this.time_offset_);
                    }

                    if (this.connected_ === false) {
                        this.emit('connected');
                        this.connected_ = true ;
                    }

                    setInterval(() => { this.sendRTTPing() ; }, this.rtt_interval_);
                }
                else {
                    this.logger_.log(NTMessageType.Warning, "received RTT packet with non-integer payload - type '%s'", 
                                    NTValue.typeString(msg.value_.type_));
                }
            }
            else {
                if (!this.topics_.setValue(msg)) {
                    this.logger_.log(NTMessageType.Warning, "received value for unknown topic, id %d", msg.id_);
                }
                else {
                    let name: string | undefined = this.topics_.getNameFromId(msg.id_);
                    if (name !== undefined) {
                        this.logger_.log(NTMessageType.Warning, "set value for topic name '%s'", name);
                    }
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

            let pubuid: number = -1 ;

            if (params['pubuid'] !== undefined) {
                pubuid = params['pubuid'] as number ;
            }

            let topic: NTTopic = this.topics_.add(params['name'], params['id'], pubuid) ;

            if (params['properties'] !== undefined) {
                topic.addProps(params['properties']) ;
            }

            this.emit('announce', topic);
        } else if (method === 'unannounce') {
            if (!this.checkvalid(params, 'name', 'string'))
                return ;

            if (!this.checkvalid(params, 'id', 'number'))
                return ;
            
            this.topics_.remove(params['name'], params['id']) ;

        } else if (method === 'properties') {
            //
            // NOP for now
            //
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
}
