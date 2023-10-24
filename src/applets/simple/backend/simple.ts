import { XeroApp } from "../../../xeroboard/XeroApp";
import { XeroApplet } from "../../../xeroboard/XeroApplet";

export class simple extends XeroApplet {
    public constructor(mainapp: XeroApp) {
        super(mainapp, "simple");
    }
}