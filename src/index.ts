import { simple } from './applets/simple/backend/simple';
import { tree } from './applets/tree/backend/tree';
import { XeroApp } from './xeroboard/XeroApp' ;

const appid: string = 'xeroboard' ;
const ntaddr: string = '127.0.0.1' ;
const ntport: number = 5810 ;
const restaddr: string = '127.0.0.1' ;
const restport: number = -1 ;

const ntapp = new XeroApp() ;
// ntapp.addApplet(new simple());
ntapp.addApplet(new tree());
ntapp.start(appid, ntaddr, ntport, restaddr, restport) ;