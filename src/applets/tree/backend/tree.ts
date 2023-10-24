import express, { Response, Request } from 'express';
import { XeroApplet } from "../../../xeroboard/XeroApplet";
import { NTLooseObject } from '../../../nt4/NTTopic';
import { NTTopicNode } from '../../../nt4/NTTopicNode';
import { NTClient } from '../../../nt4/NTClient';
import { XeroApp } from '../../../xeroboard/XeroApp';

export class tree extends XeroApplet {
    public constructor(mainapp: XeroApp) {
        super(mainapp, "tree");
    }

    public start(nettable: NTClient, server: express) {
        super.start(nettable, server) ;

        this.registerRestRequest('tables', (req, res) => { this.tables(req, res) ; }) ;

        //
        // Get all topics and their values
        //
        this.getNetTables()!.subscribe(["/"], 
        {
            topicsonly: false,
            prefix: true
        }) ;
    }

    public getTitle() : string {
        return "NT Tree View" ;
    }

    private createLooseObj(node: NTTopicNode) : NTLooseObject {
        let loose: NTLooseObject = {} ;
        loose['title'] = node.name ;
        loose['dtype'] = node.typeString ;
        loose['value'] = node.valueString;

        if (node.children_.length != 0) {
            let children: NTLooseObject[] = [] ;
            for(let child of node.children_) {
                children.push(this.createLooseObj(child));
            }
            loose['children'] = children ;
        }
        return loose ;
    }

    private tables(req: Request<{}, any, any, any, Record<string, any>>, res: Response<any, Record<string, any>>) {
        let ret : NTLooseObject[] = [] ;

        let top = this.getNetTables()!.getTop() ;
        if (top) {
            for(let child of top.children_) {
                ret.push(this.createLooseObj(child));
            }
        }

        res.json(ret) ;
    }
}