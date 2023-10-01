import { NTValue } from "./NTValue";

export class NTTopic {
    public name_ : string ;
    public id_ : number ;
    public value_ : NTValue | null ;

    public constructor(name: string, id: number) {
        this.name_ = name ;
        this.id_ = id ;
        this.value_ = null ;
    }

    public addProps(props) {
    }
}