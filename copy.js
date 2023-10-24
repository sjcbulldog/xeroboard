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
    let destpath = path.join(destdir, applet) ;

    console.log("  Processing applet '" + applet + "'");
    fs.rmSync(destpath, { recursive : true, force : true});
    if (fs.existsSync(destpath)) {
        console.log("    Cannot remove '" + applet + "' - dir '" + destpath + "'");
        process.exit(1);
    }

    files = fs.readdirSync(fullpath);
    for(let file of files) {
        if (file === 'backend' || file === 'frontend') {
            //
            // These directories are type scripts and are handled independently
            // 
            continue ;
        }

        let srcdir = path.join(fullpath, file) ;
        let dest = path.join(destpath, file) ;
        try {
            fse.copySync(srcdir, dest) ;
        }
        catch(err) {
            console.log("Cannot process applet '" + applet + "' : directory '" + file + "' - " + err);
            process.exit(1);
        }
    }

    let srcdir = path.join(builddir, applet, 'frontend');
    let dest = path.join(destpath, 'js');
    if (fs.existsSync(srcdir)) {
        try { 
            fse.copySync(srcdir, dest, (src, dest) => { 
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

function copyAppFiles() {
    console.log("Copying main 'app' files")
    const src = path.join(__dirname, 'src', 'app');
    const dest = path.join(__dirname, 'content', 'app');

    try { 
        fse.copySync(src, dest) ;
    }
    catch(err) {
        console.log("    Cannot copy main 'app' files: " + err) ;
        console.log("      src: " + src) ;
        console.log("      dest: " + dest + "'");
        process.exit(1);
    }
}

function copyAll() {
    let files = fs.readdirSync(basedir) ;
    console.log("Copying applet files");
    for(let file of files) {
        copyApplet(file) ;
    }

    copyAppFiles() ;
}

copyAll() ;

if (count === 0) {
    console.log("No applets were copied sucessfully") ;
    process.exit(1);
} else {
    console.log(count + " applets were processed");
}
