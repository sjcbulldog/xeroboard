import { NTMessageLogger, NTMessageType } from "./NTMessageLogger";
import { NTTopic } from "./NTTopic";

class NTTopicNode {
    public name_ : string ;
    public children_ : NTTopicNode[] ;    // TODO: Convert to map to go faster?
    public topic_ : NTTopic | null ;

    constructor(name: string) {
        this.name_ = name ;
        this.children_ = [] ;
        this.topic_ = null ;
    }

    public findChildByName(name: string) {
        return this.children_.find((elem) => elem.name_ === name);
    }
}

export class NTTopicTree {
    private top_ : NTTopicNode ;
    private logger_ : NTMessageLogger ; 

    constructor(logger: NTMessageLogger, ) {
        this.top_ = new NTTopicNode("");
        this.logger_ = logger ;
    }

    private dumpOneNode(node: NTTopicNode, indent: number) {
        let str: string = ' '.repeat(indent) + node.name_ ;
        this.logger_.log(NTMessageType.Debug, str) ;
        for(let child of node.children_) {
            this.dumpOneNode(child, indent + 2) ;
        }
    }

    public dumpTree() {
        this.logger_.log(NTMessageType.Debug, "Dumping Topic Tree");
        this.logger_.log(NTMessageType.Debug, "=================================================");
        for(let node of this.top_.children_) {
            this.dumpOneNode(node, 0) ;
        }
        this.logger_.log(NTMessageType.Debug, "=================================================");
    }

    public add(name: string, id: number) : NTTopic {
        const namearr : string[] = name.split('/');

        if (namearr[0].length == 0) {
            //
            // Remove the leading empty value, as this is associated with the
            // leading slash in the name
            //
            namearr.shift() ;
        }

        const node: NTTopicNode = this.findOrCreateNodes(this.top_, namearr) ;
        const topic: NTTopic = new NTTopic(node.name_, id) ;
        node.topic_ = topic ;

        return topic ;
    }

    private findOrCreateNodes(node: NTTopicNode, names: string[]) : NTTopicNode {
        let ret: NTTopicNode ;
        let child: NTTopicNode | undefined = node.findChildByName(names[0]) ;

        if (child === undefined) {
            child = new NTTopicNode(names[0]) ;
            node.children_.push(child) ;
        }

        if (names.length > 1) {
            names.shift() ;
            ret = this.findOrCreateNodes(child, names) ;
        }
        else {
            ret = child ;
        }

        return ret;
    }
}
