
export class NTSubscribeOptions
{
    public periodic? : number ;
    public all? : boolean ;
    public topicsonly?: boolean ;
    public prefix?: boolean ;
}

export class NTPublishOptions {
    public persistent?: boolean ;
    public retained?: boolean ;
}