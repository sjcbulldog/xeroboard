let path = require('path');
let fs = require('fs');
let fse = require('fs-extra');
let process = require('process');

const basedir = path.join(__dirname, 'src', 'applets');
const destdir = path.join(__dirname, 'content', 'applets');
const builddir = path.join(__dirname, 'build', 'applets') ;
let count = 0 ;

if (!fs.existsSync(basedir)) {
    console.log("The applet directory '" + basedir + "' does not exist") ;
    process.exit(1);
}

function copyApplet(applet) {
    let fullpath = path.join(basedir, applet) ;

    console.log("  Processing applet '" + applet + "'");
    fs.rmSync(fullpath, { recursive : true, force : true});
    if (fs.existsSync(fullpath)) {
        console.log("    Cannot remove '" + applet + "' - dir '" + fullpath + "'");
        process.exit(1);
    }

    fs.readdir(fullpath, (err, files) => {
        if (err) {
            console.log("Cannot process applet '" + applet + "' - " + err);
            retrurn ;
        }

        for(let file of files) {
            if (file === 'backend' || file === 'frontend') {
                //
                // These directories are type scripts and are handled independently
                // 
                continue ;
            }

            try {
                fse.copySync(srcdir, destdir) ;
            }
            catch(err) {
                console.log("Cannot process applet '" + applet + "' : directory '" + file + "' - " + err);
                process.exit(1);
            }
        }
    }) ;

    srcpath = path.join(builddir, applet, 'frontend');
    destpath = path.join(fullpath, applet, 'js');
        if (fs.existsSync(srcpath)) {
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
    }
    count++ ;
}

function copyAll() {

    let files = fs.readdirSync(basedir) ;
    console.log("basedir: " + basedir);
    console.log("files: " + files);

    console.log("Copying applet files");
    for(let file of files) {
        copyApplet(file) ;
    }
}

copyAll() ;

if (count === 0) {
    console.log("No applets were copied sucessfully") ;
    process.exit(1);
} else {
    console.log(count + " applets were processed");
}
