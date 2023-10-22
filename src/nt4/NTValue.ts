

export enum NTType {
    NT_UNASSIGNED = 0x00,
    NT_BOOLEAN = 0x01,
    NT_DOUBLE = 0x02,
    NT_STRING = 0x04,
    NT_RAW = 0x08,
    NT_BOOLEAN_ARRAY = 0x10,
    NT_DOUBLE_ARRAY = 0x20,
    NT_STRING_ARRAY = 0x40,
    NT_RPC = 0x80,
    NT_INTEGER = 0x100,
    NT_FLOAT = 0x200,
    NT_INTEGER_ARRAY = 0x400,
    NT_FLOAT_ARRAY = 0x800
}
export class NTValue {
    public type_ : NTType ;
    private value_ : any ;

    public constructor(v? : any) {

        if (v == undefined) {
            this.type_ = NTType.NT_UNASSIGNED ;
            this.value_ = null ;
        }
        else if (typeof v === "number") {
            this.type_ = NTType.NT_DOUBLE ;
            this.value_ = v ;
        }
        else if (typeof v === "string") {
            this.type_ = NTType.NT_STRING ;
            this.value_ = v ;
        }
        else if (typeof v === "boolean") {
            this.type_ = NTType.NT_BOOLEAN ;
            this.value_ = v ;
        }
        else {
            throw new Error("NTvalue.constructor() - invalid type") ;
        }
    }

    public initArray(type: NTType) {
        this.type_ = type
        this.value_ = [] ;
    }

    public initRaw(data: Uint8Array) {
        this.type_ = NTType.NT_RAW ;
        this.value_ = data ;
    }

    public initInteger(value: number) {
        this.type_ = NTType.NT_INTEGER ;
        this.value_ = value ;
    }

    public initFloat(value: number) {
        this.type_ = NTType.NT_FLOAT ;
        this.value_ = value ;
    }    
    
    public initBoolean(value: boolean) {
        this.type_ = NTType.NT_BOOLEAN ;
        this.value_ = value ;
    }   

    public initDouble(value: number) {
        this.type_ = NTType.NT_DOUBLE ;
        this.value_ = value ;
    }   

    public initString(value: string) {
        this.type_ = NTType.NT_STRING ;
        this.value_ = value ;
    }   

    public isInteger() : boolean {
        return this.type_ == NTType.NT_INTEGER ;
    }

    public isDouble() : boolean {
        return this.type_ == NTType.NT_DOUBLE ;
    }    

    public isString() : boolean {
        return this.type_ == NTType.NT_STRING ;
    }
    
    public isFloat() : boolean {
        return this.type_ == NTType.NT_FLOAT ;
    } 
    
    public isBoolean() : boolean {
        return this.type_ == NTType.NT_BOOLEAN ;
    }

    public static typeString(type: NTType) : string {
        let ret: string = "" ;

        switch(type) {
            case NTType.NT_UNASSIGNED:
                ret = "unassigned" ;
                break ;
            case NTType.NT_BOOLEAN:
                ret = "boolean" ;
                break ;                
            case NTType.NT_DOUBLE:
                ret = "double" ;
                break ;                    
            case NTType.NT_STRING:
                ret = "string" ;
                break ;                    
            case NTType.NT_RAW:
                ret = "raw" ;
                break ;                    
            case NTType.NT_BOOLEAN_ARRAY:
                ret = "boolean[]" ;
                break ;                    
            case NTType.NT_DOUBLE_ARRAY:
                ret = "double[]" ;
                break ;                    
            case NTType.NT_STRING_ARRAY:
                ret = "string[]" ;
                break ;                    
            case NTType.NT_INTEGER:
                ret = "int" ;
                break ;                    
            case NTType.NT_FLOAT:
                ret = "float" ;
                break ;                    
            case NTType.NT_INTEGER_ARRAY:
                ret = "int[]" ;
                break ;                    
            case NTType.NT_FLOAT_ARRAY:
                ret = "float[]" ;
                break ;                  
            default:
                ret = "invalid" ;
                break ;
        }

        return ret ;
    }

    public asBoolean() : boolean {
        if (typeof this.value_ !== 'boolean')
            throw new Error('asBoolean accessing non-boolean type') ;

        return this.value_ as boolean ;
    }

    public asDouble() : number {
        if (typeof this.value_ !== 'number')
            throw new Error('asDouble accessing non-double type') ;

        return this.value_ as number;
    }      

    public asString() : string {
        if (typeof this.value_ !== 'string')
            throw new Error('asString accessing non-string type') ;

        return this.value_ as string ;
    }

    public asBooleanArray() : boolean[] {
        if (Array.isArray(this.value_) && this.value_.every(item => typeof item === 'boolean'))
            return this.value_ ;

        throw new Error('asBooleanArray accessing non-boolean array type') ;
    }

    public asDoubleArray() : number[] {
        if (Array.isArray(this.value_) && this.value_.every(item => typeof item === 'number'))
            return this.value_ ;

        throw new Error('asDoubleArray accessing non-double array type') ;
    }    
  
    public asStringArray() : number[] {
        if (Array.isArray(this.value_) && this.value_.every(item => typeof item === 'string'))
            return this.value_ ;

        throw new Error('asStringArray accessing non-string array type') ;
    }      

    public asInteger() : number {
        if (typeof this.value_ !== 'number')
            throw new Error('asInteger accessing non-integer type') ;

        return Math.trunc(this.value_ as number) ;
    }

    public asFloat() : number {
        if (typeof this.value_ !== 'number')
            throw new Error('asFloat accessing non-float type') ;

        return this.value_ as number;
    }  

    public asRaw() : Uint8Array {
        if (!(this.value_ instanceof Uint8Array))
            throw new Error('asRaw accessing non-raw type');

        return this.value_ as Uint8Array ;
    }

    public asIntegerArray() : number[] {
        if (Array.isArray(this.value_) && this.value_.every(item => typeof item === 'number')) {
            let ret : number[] = [] ;
            ret.forEach((element) => {
                ret.push(Math.trunc(element));
            }) ;
            return ret ;
        }

        throw new Error('asDoubleArray accessing non-double array type') ;
    }   
    
    public asFloatArray() : number[] {
        if (Array.isArray(this.value_) && this.value_.every(item => typeof item === 'number'))
            return this.value_ ;

        throw new Error('asDoubleArray accessing non-double array type') ;
    }     

    public unpack(data: any[], index: number) {
        if (typeof data[index] !== 'number') {
            throw new Error('unpack - invalid data for value type - not a number') ;
        }

        const type: number = data[index] ;

        switch(type) {
            case 0:     // NTType.NT_BOOLEAN
                if (typeof data[index + 1] !== 'boolean') {
                    throw new Error('unpack - invalid data payload for type NT_BOOLEAN') ;
                }
                this.initBoolean(data[index + 1]);
                break ;
            case 1:     // NTType.NT_DOUBLE
                if (typeof data[index + 1] !== 'number') {
                    throw new Error('unpack - invalid data payload for type NT_DOUBLE') ;
                }
                this.initDouble(data[index + 1]);
                break ;
            case 2:     // NTType.NT_INTEGER
                if (typeof data[index + 1] !== 'number') {
                    throw new Error('unpack - invalid data payload for type NT_INTEGER') ;
                }
                this.initInteger(data[index + 1]);
                break ;
            break ;            
            case 3:     // NTType.NT_FLOAT
                if (typeof data[index + 1] !== 'number') {
                    throw new Error('unpack - invalid data payload for type NT_INTEGER') ;
                }
                this.initInteger(data[index + 1]);
                break ;            
            case 4:     // NTType.NT_STRING
                if (typeof data[index + 1] !== 'string') {
                    throw new Error('unpack - invalid data payload for type NT_STRING') ;
                }
                this.initString(data[index + 1]);
                break ;            
            case 5:     // NTType.NT_RAW
                break ;

            case 16:    // NTType.NT_BOOLEAN_ARRAY
            case 17:    // NTType.NT_DOUBLE_ARRAY            
            case 18:    // NTType.NT_INTEGER_ARRAY
            case 19:    // NTType.NT_FLOAT_ARRAY
            case 20:    // NTType.NT_STRING_ARRAY
                this.initArray(NTType.NT_STRING_ARRAY) ;
                let dataarray = data[3] as [] ;
                dataarray.forEach( (entry) => {
                    this.value_.push(entry) ;
                }) ;
                break;
                
            default:
                throw new Error("NTValue.unpack() - invalid data type") ;
                break ;
        }
    }
    
    public pack(packet: any[]) : void {
        switch(this.type_) {
            case NTType.NT_BOOLEAN:
                packet.push(0) ;
                packet.push(this.asBoolean());
                break;

            case NTType.NT_INTEGER:
                packet.push(2);
                packet.push(this.asInteger());
                break ;
            
            case NTType.NT_FLOAT:
                packet.push(3) ;
                packet.push(this.asFloat());
                break ;

            case NTType.NT_DOUBLE:
                packet.push(1) ;
                packet.push(this.asDouble());
                break; 

            case NTType.NT_STRING:
                packet.push(4) ;
                packet.push(this.asString());
                break ;

            case NTType.NT_RAW:
            case NTType.NT_RPC:
                packet.push(5) ;
                packet.push(this.asRaw());
                break ;

            case NTType.NT_BOOLEAN_ARRAY:
                {
                    let d = this.asBooleanArray() ;
                    packet.push(16) ;
                    packet.push(d.length);
                    d.forEach((entry) => {  packet.push(entry) }) ;
                    break ;
                }

            case NTType.NT_INTEGER_ARRAY:
                {
                    let d = this.asIntegerArray() ;
                    packet.push(18) ;
                    packet.push(d.length);
                    d.forEach((entry) => {  packet.push(entry) }) ;
                    break ;
                }

            case NTType.NT_FLOAT_ARRAY:
                {
                    let d = this.asFloatArray() ;
                    packet.push(19) ;
                    packet.push(d.length);
                    d.forEach((entry) => {  packet.push(entry) }) ;
                    break ;
                }   
                
            case NTType.NT_DOUBLE_ARRAY:
                {
                    let d = this.asDoubleArray() ;
                    packet.push(17) ;
                    packet.push(d.length);
                    d.forEach((entry) => {  packet.push(entry) }) ;
                    break ;
                }

            case NTType.NT_STRING_ARRAY:
                {
                    let d = this.asStringArray() ;
                    packet.push(20) ;
                    packet.push(d.length);
                    d.forEach((entry) => {  packet.push(entry) }) ;
                    break ;
                }

            default:
                throw new Error("NTValue.pack() - invalid value type") ;
                break;
        }
    }

    public toString() : string {
        let ret : string = this.value_.toString() ;
        return ret ;
    }
}