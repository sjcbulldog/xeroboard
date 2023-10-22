import { NTTimedValue, NTTopic } from "./NTTopic";
import { NTValue } from "./NTValue";

export class NTTopicNode {
    public name_ : string ;
    public children_ : NTTopicNode[] ;    // TODO: Convert to map to go faster?
    public topic_ : NTTopic | null ;

    constructor(name: string) {
        this.name_ = name ;
        this.children_ = [] ;
        this.topic_ = null ;
    }

    public get hasValue() : boolean {
        return this.topic_ !== null && this.topic_.getValue() != null ;
    }
    
    public get value() : NTTimedValue | null {
        return this.topic_!.getValue() ;
    }

    public get valueString() : string {
        return this.hasValue ? this.topic_!.getValue()!.value.toString() : "" ;
    }    

    public get typeString() : string {
        return this.hasValue ? NTValue.typeString(this.topic_!.getValue()!.value.type_) : "" ;
    }   

    public findChildByName(name: string) {
        return this.children_.find((elem) => elem.name_ === name);
    }
}
