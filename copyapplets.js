let path = require('path');
let fs = require('fs');
let fse = require('fs-extra');
let process = require('process');

const basedir = path.join(__dirname, 'src', 'applets');
const destdir = path.join(__dirname, 'content', 'applets');
const builddir = path.join(__dirname, 'build', 'applets') ;

if (!fs.existsSync(basedir)) {
    console.log("The applet directory '" + basedir + "' does not exist") ;
    process.exit(1);
}

console.log("Copying applet files");
fs.readdir(basedir, (err, files) => { 
    if (err) {
        console.log("Cannot process applets: " + err);
        retrurn ;
    }

    let count = 0 ;
    for(let file of files) {
        console.log("  Processing applet '" + file + "'");
        const destapplet = path.join(destdir, file) ;
        fs.rmSync(destapplet, { recursive : true, force : true});
        if (fs.existsSync(destapplet)) {
            console.log("    Cannot remove directory '" + destapplet + "'");
            process.exit(1);
        }

        //
        // Copy HTML files
        //
        let srcpath = path.join(basedir, file, 'html');
        let destpath = path.join(destdir, file, 'html');
        try { 
            fse.copySync(srcpath, destpath);
        }
        catch(err) {
            console.log("    Cannot copy HTML files: " + err);
            console.log("      src: " + srcpath) ;
            console.log("      dest: " + destpath + "'");
            process.exit(1);
        }

        //
        // Copy JS files
        //
        srcpath = path.join(basedir, file, 'frontend');
        destpath = path.join(destdir, file, 'js');
        try { 
            fse.copySync(srcpath, destpath, (src, dest) => { 
                return path.extname(src) === ".js" || fs.lstatSync(src).isDirectory();
            });
        }
        catch(err) {
            console.log("    Cannot copy JavaScript files: " + err);
            console.log("      src: " + srcpath) ;
            console.log("      dest: " + destpath + "'");
            process.exit(1);
        }

        //
        // Copy built TS files
        //
        srcpath = path.join(builddir, file, 'frontend');
        destpath = path.join(destdir, file, 'js');
        try { 
            fse.copySync(srcpath, destpath, (src, dest) => { 
                return path.extname(src) === ".js"  || fs.lstatSync(src).isDirectory();
            });
        }
        catch(err) {
            console.log("    Cannot copy TypeScript built files: " + err) ;
            console.log("      src: " + srcpath) ;
            console.log("      dest: " + destpath + "'");
            process.exit(1);
        }        

        count++ ;
    }

    if (count === 0) {
        console.log("No applets were copied sucessfully") ;
        process.exit(1);
    } else {
        console.log(count + " applets were processed");
    }
}) ;
