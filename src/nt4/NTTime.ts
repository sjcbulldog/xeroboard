
export function NTCurrentTime() : number {
    let d: Date = new Date() ;

    //
    // Return the time in uSeconds since jan 1, 1970
    //
    return d.getTime() * 1000 ;
}