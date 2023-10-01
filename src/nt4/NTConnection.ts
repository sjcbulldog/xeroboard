import { RawData, WebSocket } from 'ws' ;
import { EventEmitter } from 'node:events' ;

export class NTConnection extends EventEmitter {
    private connected_ : boolean ;
    private addr_ : string ;
    private sock_ : WebSocket | null ;

    public constructor(addr: string) {
        super() ;

        this.addr_ = addr ;
        this.connected_ = false ;
        this.sock_ = null ;
    }

    public send(data: Uint8Array) : void {
        if (this.sock_ !== null) {
            this.sock_.send(data) ;
        }
    }

    public sendJson(obj: Object) : void {
        if (this.sock_ !== null) {
            this.sock_.send(obj.toString());
        }
    }

    public isConnected() : boolean {
        return this.connected_ ;
    }

    public isConnecting() : boolean { 
        return this.sock_ !== null && this.connected_ === false ;
    }

    public connect() {
        if (this.isConnected() || this.isConnecting())
            return ;
        
        this.sock_ = new WebSocket(this.addr_);
        this.sock_.on('open', () => { this.socketOpened() ; }) ;
        this.sock_.on('close', (code: number, reason: Buffer) => { this.socketClosed(code, reason) ; }) ;
        this.sock_.on('error', (err: Error) => { this.socketError(err) ; }) ;
        this.sock_.on('message', (data: RawData, isBinary: boolean) => { this.socketMessage(data, isBinary) ; }) ;
    }

    public disconnect() {
        if (this.sock_ !== null) {
            this.sock_.close() ;
            this.connected_ = false ;
            this.sock_ = null ;
        }
    }

    private socketOpened() : void {
        this.connected_ = true ;
        this.emit('connected') ;
    }

    private socketClosed(code: number, reason: Buffer) : void {
        this.connected_ = false ;
        this.sock_ = null ;
        
        this.emit('disconnected', code, reason) ;
    }

    private socketError(err: Error) : void {
        this.emit('error', err);
    }

    private socketMessage(data: RawData, isBinary: boolean) {
        if (isBinary) {
            this.emit('binary', data) ;
        }
        else {
            this.emit('json', data) ;
        }
    }
}