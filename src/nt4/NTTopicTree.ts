import { NTBinaryMessage } from "./NTBinaryMessage";
import { NTMessageLogger, NTMessageType } from "./NTMessageLogger";
import { NTTopic } from "./NTTopic";
import { NTTopicNode } from "./NTTopicNode" ;

export class NTTopicTree {
    public top_ : NTTopicNode ;
    private logger_ : NTMessageLogger ; 
    private topic_map_ : Map<number, NTTopic> ;

    constructor(logger: NTMessageLogger, ) {
        this.top_ = new NTTopicNode("");
        this.logger_ = logger ;
        this.topic_map_ = new Map<number, NTTopic>() ;        
    }

    public getTopics() : Iterable<NTTopic> {
        return this.topic_map_.values() ;
    }

    public getTopicByName(name: string) : NTTopic | null {
        let node: NTTopicNode | null = this.getTopicNodeFromName(name, false) ;
        if (node === null || node.topic_ === null)
            return null; 

        return node.topic_ ;
    }

    public getNameFromId(id: number) : string | undefined {
        let ret: string | undefined = undefined ;
        if (this.topic_map_.has(id)) {
            ret = this.topic_map_.get(id)!.name_ ;
        }

        return ret;
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

    private disableTopic(topic: NTTopic) {        
        topic.disabled() ;
        if (this.topic_map_.has(topic.subid_)) {
            this.topic_map_.delete(topic.subid_) ;
        }
    }

    public remove(name: string, id: number) {
        const namearr : string[] = name.split('/');

        if (namearr[0].length == 0) {
            //
            // Remove the leading empty value, as this is associated with the
            // leading slash in the name
            //
            namearr.shift() ;
        }

        let node: NTTopicNode | null = this.getTopicNodeFromName(name, false) ;
        if (node === null || node.topic_ === null) {
            return ;
        }

        if (node.topic_ === null) {
            if (this.topic_map_.has(id)) {
                this.logger_.log(NTMessageType.Debug, "internal error #1 - the topic is in the topic map but not in the tree");
                let topic = this.topic_map_.get(id) ;
                this.disableTopic(topic!) ;
            }
        } else if (!this.topic_map_.has(id)) {
            if (node.topic_ !== null) {
                //
                // Something is wrong.  The topic is in the tree but not
                // in the map.  This is a bug, an internal error
                //
                this.logger_.log(NTMessageType.Debug, "internal error #2 - the topic is in the tree but is not in the topic map");
                node.topic_.disabled() ;
                node.topic_ = null ;
            }
        }
        else {
            this.disableTopic(this.topic_map_.get(id)!);
        }
    }

    public add(name: string, id: number, pubid: number, all: boolean) : NTTopic {
        let node: NTTopicNode = this.getTopicNodeFromName(name, true)! ;

        const topic: NTTopic = new NTTopic(name, id, pubid, all) ;
        node.topic_ = topic ;

        this.topic_map_.set(topic.subid_, topic) ;

        this.logger_.log(NTMessageType.Debug, "added topic '"+ name + "'");        

        return topic ;
    }

    public setValue(msg: NTBinaryMessage) : boolean {
        let ret: boolean = false ;

        if (this.topic_map_.has(msg.id_)) {
            this.topic_map_.get(msg.id_)!.addValue(msg.timestamp_, msg.value_) ;
            ret = true ;
        }

        return ret;
    }

    private getTopicNodeFromName(name: string, create : boolean) : NTTopicNode | null {
        const namearr : string[] = name.split('/');

        if (namearr[0].length == 0) {
            //
            // Remove the leading empty value, as this is associated with the
            // leading slash in the name
            //
            namearr.shift() ;
        }

        return this.findOrCreateNodes(this.top_, namearr, create) ;
    }

    private findOrCreateNodes(node: NTTopicNode, names: string[], create: boolean) : NTTopicNode | null {
        let ret: NTTopicNode | null ;
        let child: NTTopicNode | undefined = node.findChildByName(names[0]) ;

        if (child === undefined) {
            if (create) {
                child = new NTTopicNode(names[0]) ;
                node.children_.push(child) ;
            }
            else {
                return null ;
            }
        }

        if (names.length > 1) {
            names.shift() ;
            ret = this.findOrCreateNodes(child, names, create) ;
        }
        else {
            ret = child ;
        }

        return ret;
    }
}
