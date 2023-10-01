import { NTMessageType } from "./NTMessageLogger";
import { NTMessageSink } from "./NTMessageSink";

export class NTConsoleSink extends NTMessageSink {
    constructor() {
        super() ;
    }

    public out(type: NTMessageType, msg: string) {
        console.log(NTMessageType[type] + ": " + msg);
    }
}