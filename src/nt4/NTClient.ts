import { RawData } from "ws";
import { NTConnection } from "./NTConnection";
import { encode, decode } from '@msgpack/msgpack' ;

export class NTClient {
    private connection_ : NTConnection ;
    private lastSent_ : number ;

    private static kMinPeriodMs : number = 5 ;

    public constructor(addr: string) {
        this.lastSent_ = 0 ;
        this.connection_ = new NTConnection(addr) ;

        this.connection_.on('connected', () => { this.connected() ; });
        this.connection_.on('disconnected', (code: number, reason: Buffer) => { this.disconnected(code, reason) ; });
        this.connection_.on('error', (err: Error) => { this.error(err) ; });
        this.connection_.on('binary', (data: RawData) => { this.binaryMessage(data)}) ;
        this.connection_.on('json', (obj: Object) => { this.jsonMessage(obj)}) ;

        this.connection_.connect() ;
    }

    private connected() : void {
        console.log("NTClient connected") ;
        this.sendInitialValues() ;
    }

    private disconnected(code: number, reason: Buffer) : void {
    }    

    private error(err: Error) {
    }

    private binaryMessage(data: RawData) {
        console.log(data) ;
    }

    private jsonMessage(msg: Object) {
        console.log(msg) ;
    }

    private sendInitialValues() {
        this.sendControl(0) ;
    }

    private sendControl(now: number) {
        if (now < this.lastSent_ + NTClient.kMinPeriodMs) {
            //
            // Rate limit enforcement
            //
            return ;
        }
    }
}


// let timest = {
//     "a" : "bcd"
// } ;

// const encoded: Uint8Array = encode(timest) ;
// this.sock_!.send(encoded);