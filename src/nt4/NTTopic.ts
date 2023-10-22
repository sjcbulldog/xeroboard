import EventEmitter from "node:events";
import { NTValue } from "./NTValue";

export interface NTLooseObject {
    [key: string]: any
}

export class NTTimedValue {
    public readonly value : NTValue ;
    public readonly timestamp: number ;

    public constructor(v: NTValue, t: number) {
        this.value = v ;
        this.timestamp = t ;
    }
}

export class NTTopic extends EventEmitter {
    public readonly name_ : string ;
    public readonly subid_ : number ;
    public readonly pubuid_ : number ;
    public valid_ : boolean ;
    public props_ : NTLooseObject ;

    private value_ : NTValue | NTValue[] | null ;
    private timestamp_ : number | number[] ;    
    private allvalues_ : boolean ;

    public constructor(name: string, subid: number, pubuid: number, allvalues: boolean) {
        super() ;

        this.name_ = name ;
        this.subid_ = subid ;
        this.value_ = null ;
        this.timestamp_ = -1 ;
        this.valid_ = true ;
        this.props_ = {} ;
        this.pubuid_ = pubuid;
        this.allvalues_ = allvalues ;
    }

    public addProps(props) {
        for(const key in props) {
            this.props_[key] = props[key] ;
        }
    }

    public addValue(timestamp: number, value: NTValue) : void {
        if (this.allvalues_) {
            if (this.value_ === null) {
                this.value_ = [] ;
                this.timestamp_ = [] ;
            }

            (this.value_ as NTValue[]).push(value) ;
            (this.timestamp_ as number[]).push(timestamp) ;
        }
        else {
            this.value_ = value ;
            this.timestamp_ = timestamp ;
        }

        this.emit("value-changed") ;
    }

    public getValue() : NTTimedValue | null {
        let ret: NTTimedValue | null = null ;
        if (this.value_ !== null) {
            if (this.allvalues_) {
                let varray : NTValue[] = (this.value_ as NTValue[]) ;
                let tarray: number[] = (this.timestamp_ as number[]);

                ret = new NTTimedValue(varray[varray.length - 1], tarray[tarray.length - 1]);
            }
            else {
                ret = new NTTimedValue(this.value_ as NTValue, this.timestamp_ as number);
            }
        }

        return ret;
    }

    public getValues() : NTTimedValue[] | null {
        let ret: NTTimedValue[] | null = null ;

        if (this.value_ !== null) {
            ret = [] ;
            if (this.allvalues_) {
                let varray : NTValue[] = (this.value_ as NTValue[]) ;
                let tarray: number[] = (this.timestamp_ as number[]);
                for(let i = 0 ; i < varray.length ; i++) {
                    ret.push(new NTTimedValue(varray[i], tarray[i]));
                }
            }
            else {
                ret.push(this.getValue()!) ;
            }
        }

        return ret ;
    }

    public disabled() : void {
        this.emit("disabled") ;
        this.valid_ = false ;
    }
}