import { NTMessageSink } from "./NTMessageSink";

export enum NTMessageType {
    Debug,
    Info,
    Warning,
    Error
} ;

export class NTMessageLogger {
    private sinkmap_ : Map<NTMessageType, NTMessageSink[]> ;

    constructor() {
        this.sinkmap_ = new Map<NTMessageType, NTMessageSink[]>() ;
    }

    public addSink(type: NTMessageType, sink: NTMessageSink) {
        if (!this.sinkmap_.has(type)) {
            this.sinkmap_.set(type, []) ;
        }

        this.sinkmap_.get(type)?.push(sink) ;
    }

    public log(type: NTMessageType, format: string, ...args) {
        if (this.sinkmap_.has(type)) {
            var args = Array.prototype.slice.call(arguments).splice(2); //get all the extra arguments (exclude the original string)
            var counter = 0; //set a counter
        
            for (var i = 0; i < args.length; i++) { //loop through the extra arguments (should match the number of placeholders in the string)
                var match = /%s|%d|%i|%f|%o/.exec(format); //regex
                if (match) { //if match found
                    var index = match.index; //store the index
                    switch (match[0]) { //check whether the placeholder has a real value with the correct type
                        case '%s': //for strings
                            format = (typeof args[counter] == 'string') ? format.substr(0, index) + args[counter] + format.substr(index + 2) : format.substr(0, index) + NaN + format.substr(index + 2);
                            break;
                        case '%d': //for numbers....
                        case '%i':
                        case '%f':
                            format = (typeof args[counter] == 'number') ? format.substr(0, index) + args[counter] + format.substr(index + 2) : format.substr(0, index) + NaN + format.substr(index + 2);
                            break;
                        case '%o': //for objects
                            format = (typeof args[counter] == 'object') ? format.substr(0, index) + args[counter] + format.substr(index + 2) : format.substr(0, index) + NaN + format.substr(index + 2);
                            break;
                    }
                }
                counter++;
            }

            this.sinkmap_.get(type)?.forEach((sink) => { sink.out(type, format)}) ;
        }
    }
}