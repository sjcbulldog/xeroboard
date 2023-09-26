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
        this.sock_.on('open', (ws: WebSocket) => { this.socketOpened(ws) ; }) ;
        this.sock_.on('close', (ws: WebSocket, code: number, reason: Buffer) => { this.socketClosed(ws, code, reason) ; }) ;
        this.sock_.on('error', (ws: WebSocket, err: Error) => { this.socketError(ws, err) ; }) ;
        this.sock_.on('message', (ws: WebSocket, data: RawData, isBinary: boolean) => { this.socketMessage(ws, data, isBinary) ; }) ;
    }

    public disconnect() {
        if (this.sock_ !== null) {
            this.sock_.close() ;
            this.connected_ = false ;
            this.sock_ = null ;
        }
    }

    private socketOpened(ws: WebSocket) : void {
        this.connected_ = true ;
        this.emit('connected') ;
    }

    private socketClosed(ws: WebSocket, code: number, reason: Buffer) : void {
        this.connected_ = false ;
        this.sock_ = null ;
        
        this.emit('disconnected', code, reason) ;
    }

    private socketError(ws: WebSocket, err: Error) : void {
        this.emit('error', err);
    }

    private socketMessage(ws: WebSocket, data: RawData, isBinary: boolean) {
        if (isBinary) {
            this.emit('binary', data) ;
        }
        else {
            this.emit('json', data) ;
        }
    }
}