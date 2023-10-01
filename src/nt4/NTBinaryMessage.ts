import { NTValue } from "./NTValue";

export class NTBinaryMessage
{
    public id_ : number ;
    public timestamp_ : number ;
    public value_ : NTValue ;

    public constructor(data: any[]) {
        if (typeof data[0] !== 'number') {
            throw new Error('invalid id value in binary message - not a number') ;
        }
        this.id_ = data[0] ;

        if (typeof data[1] !== 'number') {
            throw new Error('invalid timestamp in binary message - not a number') ;
        }
        this.timestamp_ = data[1] ;

        this.value_ = new NTValue() ;
        this.value_.unpack(data, 2);
    }
}