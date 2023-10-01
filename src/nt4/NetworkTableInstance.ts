import { NTClient } from "./NTClient";
import { NTMessageLogger, NTMessageType } from "./NTMessageLogger";
import { EventEmitter } from 'node:events' ;
import { NTSubscribeOptions } from "./NTSubscribe";
import { NTTopic } from "./NTTopic";

export enum NetworkMode {
    None,
    Client,
    Server
} ;

export class NetworkTableInstance extends EventEmitter {
    private mode_ : NetworkMode ;
    private client_ : NTClient | null ;
    private logger_ : NTMessageLogger ;

    private static glogger_ : NTMessageLogger = new NTMessageLogger() ;
    private static default_ : NetworkTableInstance | null  = null ;
    private static instances_ : NetworkTableInstance[] = [] ;

    public static get kDefaultPort4(): number { return 5810 ; }

    private constructor(logger: NTMessageLogger) {
        super();
        this.logger_ = logger ;
        this.mode_ = NetworkMode.None ;
        this.client_ = null ;
    }

    public static getDefault(logger? : NTMessageLogger) {
        if (NetworkTableInstance.default_ === null) {
            NetworkTableInstance.default_ = NetworkTableInstance.create(logger);
        }

        return NetworkTableInstance.default_ ;
    }

    public static create(logger? : NTMessageLogger) {
        if (logger === undefined)
            logger = NetworkTableInstance.glogger_ ;

        const inst = new NetworkTableInstance(logger) ;
        NetworkTableInstance.instances_.push(inst);
        return inst ;
    }

    public getLogger() : NTMessageLogger {
        return this.logger_ ;
    }

    public setServer(id: string, host: string, port: number) : void {
        if (this.mode_ !== NetworkMode.None) {
            this.logger_.log(NTMessageType.Warning, "calling startClient4() when instance network mode is already defined");
            return ;
        }

        let addr = "ws://" + host + ":" + port + "/nt/" + id ;
        this.client_ = new NTClient(this.logger_, addr);
        this.client_.on("connected", () => { this.emit("connected") ;}) ;
        this.client_.on("announce", (topic: NTTopic) => { this.emit("account", topic) ; }) ;
    }

    public stopClient() : void {
        if (this.client_ !== null) {
            this.client_.close();
            this.client_ = null ;
        }
    }

    public subscribe(topic: string, options? : NTSubscribeOptions) : number {
        let ret: number = -1 ;

        if (this.client_ !== null && this.client_.isConnected()) {
            ret = this.client_.subscribe([topic], options) ;
        }

        return ret;
    }

    public subscribeMulti(topics: string[], options? : NTSubscribeOptions) : number {
        let ret: number = -1 ;

        if (this.client_ !== null && this.client_.isConnected()) {
            ret = this.client_.subscribe(topics, options) ;
        }

        return ret;
    }    
}