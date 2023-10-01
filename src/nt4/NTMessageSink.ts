import { NTMessageType } from "./NTMessageLogger";

export abstract class NTMessageSink {
    abstract out(type: NTMessageType, msg: string) ;
}