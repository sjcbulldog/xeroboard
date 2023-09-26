import { RawData, WebSocket } from 'ws' ;

export class NTConnection {
    private open_ : boolean ;
    private sock_ : WebSocket | null ;

    public constructor(addr: string) {
        this.open_ = false ;

        try {
            this.sock_ = new WebSocket(addr);
        }
        catch (err) {
            let error: Error = err as Error ;
            console.log(error) ;
            this.sock_ = null ;
            return ;
        }

        this.sock_.on('open', () => { this.socketOpened() ; }) ;
        this.sock_.on('close', () => { this.socketClosed() ; }) ;
        this.sock_.on('connection', () => { this.socketConnection() ; }) ;
        this.sock_.on('error', (err) => { this.socketError(err) ; }) ;
        this.sock_.on('message', (data) => { this.socketMessage(data) ; }) ;
    }

    private socketOpened() : void {
        this.open_ = false ;
    }

    private socketClosed() : void {
        this.open_ = false ;
    }

    private socketConnection() : void {
    }

    private socketError(err: Error) : void {
    }

    private socketMessage(data: RawData) {
    }
}