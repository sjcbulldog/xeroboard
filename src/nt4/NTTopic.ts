import EventEmitter from "node:events";
import { NTValue } from "./NTValue";

export interface NTLooseObject {
    [key: string]: any
}

export class NTTopic extends EventEmitter {
    public readonly name_ : string ;
    public readonly id_ : number ;
    public readonly pubuid_ : number ;
    public value_ : NTValue | null ;
    public timestamp_ : number ;
    public valid_ : boolean ;
    public props_ : NTLooseObject ;

    public constructor(name: string, id: number, pubuid: number) {
        super() ;

        this.name_ = name ;
        this.id_ = id ;
        this.value_ = null ;
        this.timestamp_ = -1 ;
        this.valid_ = true ;
        this.props_ = {} ;
        this.pubuid_ = pubuid;
    }

    public addProps(props) {
        for(const key in props) {
            this.props_[key] = props[key] ;
        }
    }

    public setValue(timestamp: number, value: NTValue) : void {
        this.value_ = value ;
        this.timestamp_ = timestamp ;
        this.emit("value-changed") ;
    }

    public disabled() : void {
        this.emit("disabled") ;
        this.valid_ = false ;
    }
}