'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});


// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ********************

//Sample endpoint
// app.get('/someendpoint', function(req , res){
//   let retStr = req.query.name1 + " " + req.query.name2;
//   res.send({
//     foo: retStr
//   });
// });



let sharedLib = ffi.Library('./libgpxparse',{
  'fileLogWrapper':['string',['string']],
  'routeViewPanelWrapper':['string',['string']],
  'trackViewPanelWrapper':['string',['string']],
  'attributeRouteWrapper':['string',['string','int']],
  'attributeTrackWrapper':['string',['string','int']],
  'writeWrapper':['int',['string','float','string']],
  'validateWrapper':['int',['string']],
  'addRouteWrapper':['int',['string','float','float','int']],
  'getBetweenRouteWrapper':['string',['string','float','float','float','float','float']],
  'getBetweenTrackWrapper':['string',['string','float','float','float','float','float']],
  'renameRouteWrapper':['string',['string','string','int']],
  'renameTrackWrapper':['string',['string','string','int']],
  'pointTable':['string',['string']]
});

app.get('/serverFiles', function(req, res){
  let fileFolder = path.join(__dirname+'/uploads');
  fs.readdir(fileFolder, function(err,files){
    res.send(files);
  });
});

app.get('/gpxJSON',function(req, res){
  let string = req.query.filename;
  let gpxJSON = sharedLib.fileLogWrapper('uploads/'+string);
  let obj = JSON.parse(gpxJSON);
  res.send(obj);
});

app.get('/gpxViewPanelRoute',function(req, res){
  let string = req.query.filename;
  //console.log("were in here "+string2);
  let gpxRoute = sharedLib.routeViewPanelWrapper('uploads/'+string);
//  console.log(gpxJSON);
  let obj = JSON.parse(gpxRoute);
  //console.log(obj);
  res.send(obj);
});
app.get('/gpxViewPanelTrack',function(req, res){
  let string = req.query.filename;
  //console.log("were in here "+string2);
  let gpxTrack = sharedLib.trackViewPanelWrapper('uploads/'+string);
//  console.log(gpxJSON);
  let obj = JSON.parse(gpxTrack);
  //console.log(obj);
  res.send(obj);
});
app.get('/gpxViewRouteAttr',function(req, res){
  let string = req.query.filename;
  let count = req.query.num;
  //console.log("were in here "+string2);
  let gpxRouteAttr = sharedLib.attributeRouteWrapper('uploads/'+string,count);
  let obj = JSON.parse(gpxRouteAttr);
  console.log(obj);
  res.send(obj);
});
app.get('/gpxViewTrackAttr',function(req, res){
  let string = req.query.filename;
  let count = req.query.num;
  //console.log("were in here "+string2);
  let gpxTrackAttr = sharedLib.attributeTrackWrapper('uploads/'+string, count);
  //console.log(gpxTrackAttr);
  let obj = JSON.parse(gpxTrackAttr);
  console.log(obj);
  res.send(obj);
});
app.get('/writeNewGPX',function(req, res){
  console.log(req.query.filename);
  // console.log(req.query.creator);
  // console.log(req.query.version);
  let file = req.query.filename;
  let version = req.query.version;
  //console.log(version);
  //let string = req.query.filename;
  let yesorno = sharedLib.writeWrapper(file, version, req.query.creator);
  var sucFail;
  console.log(yesorno);
  if(yesorno == 1){
    sucFail = 'success';
  }
  else{
    sucFail = 'fail';
  }
  console.log(sucFail+"created successfully");
  res.send(sucFail);
});

app.get('/checkValid',function(req, res){//this shit doesnt work so
  let string = req.query.filename;
  let toReturn = 'fail';

  let yesorno = sharedLib.validateWrapper('uploads/'+string);
  // console.log(yesorno);
  if(yesorno == 1){
    // console.log("success");
     toReturn = 'success';
  }
  else if(yesorno == 0){
    console.log(string+' is an invalid file');
    toReturn = 'fail';
    fs.unlinkSync('./uploads/'+string);
  }
  res.send(toReturn);
});

app.get('/gpxRenameRoute',function(req,res){
  // let broken = "This doesn't work :)";
  // //res.send(broken, 200);
  // res.status(200).send(broken);
  // console.log('idk')
  // res.end();
  let file = req.query.filename;
  let name = req.query.name;
  let count = req.query.index;
  console.log(count+' '+name);
  let returned = sharedLib.renameRouteWrapper('uploads/'+file,name,count);

  res.send(returned);
});

app.get('/gpxRenameTrack',function(req,res){
  // let broken = "This doesn't work :)";
  // //res.send(broken, 200);
  // res.status(200).send(broken);
  // console.log('idk')
  // res.end();
  let file = req.query.filename;
  let name = req.query.name;
  let count = req.query.index;
  console.log(count+' '+name);
  let returned = sharedLib.renameTrackWrapper('uploads/'+file,name,count);

  res.send(returned);
});

app.get('/addRouteWrap',function(req,res){

  console.log(req.query.filename + req.query.lat + req.query.lon)
  let fileString = req.query.filename;
  let lat = req.query.lat;
  let lon = req.query.lon;
  let num = req.query.value;

  let yesorno = sharedLib.addRouteWrapper('uploads/'+fileString, lat, lon,num);
  if(yesorno == 1){
    var str = "Success";
  }
  else{
    var str = "fail";
  }
  res.send(str);
  res.end();
});

app.get('/findRoutePath',function(req,res){


  let fileString = req.query.filename;
  let latS = req.query.latStart;
  let lonS = req.query.lonStart;
  let latE = req.query.latEnd;
  let lonE = req.query.lonEnd;
  let comparePoint = req.query.delta;

  let findPath = sharedLib.getBetweenRouteWrapper('uploads/'+fileString, latS, lonS, latE, lonE, comparePoint);
  console.log('Route '+findPath);
  let obj = JSON.parse(findPath);

  res.send(obj);
});
app.get('/findTrackPath',function(req,res){

  let fileString = req.query.filename;
  let latS = req.query.latStart;
  let lonS = req.query.lonStart;
  let latE = req.query.latEnd;
  let lonE = req.query.lonEnd;
  let comparePoint = req.query.delta;

  let findPath = sharedLib.getBetweenTrackWrapper('uploads/'+fileString, latS, lonS, latE, lonE, comparePoint);
  let obj = JSON.parse(findPath);

  res.send(obj);
});

//DataBase Stuff
let dbConf = {};
app.get('/connectDB',async function(req,res){

  dbConf = {
  host     : 'dursley.socs.uoguelph.ca',
  user     : req.query.username,
  password : req.query.passwrd,
  database : req.query.dbname
  };



var yesorno;

let checkFile = "select TABLE_NAME from INFORMATION_SCHEMA.TABLES where TABLE_NAME = 'FILE'";
let checkRoute = "select TABLE_NAME from INFORMATION_SCHEMA.TABLES where TABLE_NAME = 'ROUTE'";
let checkPoint = "select TABLE_NAME from INFORMATION_SCHEMA.TABLES where TABLE_NAME = 'POINT'";

let createFile = "CREATE TABLE FILE(gpx_id INT AUTO_INCREMENT PRIMARY KEY,file_name VARCHAR(60) NOT NULL,ver DECIMAL(2,1) NOT NULL,creator VARCHAR(256) NOT NULL)";
let createRoute = "CREATE TABLE ROUTE(route_id INT AUTO_INCREMENT PRIMARY KEY,route_name VARCHAR(256),route_len FLOAT(15,7) NOT NULL,gpx_id INT NOT NULL,FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE)";
let createPoint = "CREATE TABLE POINT(point_id INT AUTO_INCREMENT PRIMARY KEY,point_index INT NOT NULL,latitude DECIMAL(11,7) NOT NULL,longitude DECIMAL(11,7) NOT NULL,point_name VARCHAR(256),route_id INT NOT NULL,FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE)";


async function connectToDb() {
  // get the client
  const mysql = require('mysql2/promise');

  let connection;
  let tf = '1';

  try{
      // create the connection
      connection = await mysql.createConnection(dbConf)


      // let string = '{"name":[{"data":"whatev"}]}';
      // string = JSON.parse(string);


      const [check] = await connection.execute(checkFile);
      if(check.length <= 0 ){
        await connection.execute(createFile);
      }
      const [check2] = await connection.execute(checkRoute);
      if(check2.length <= 0 ){
        await connection.execute(createRoute);
      }
      const [check3] = await connection.execute(checkPoint);
      if(check3.length <= 0 ){
        await connection.execute(createPoint);
      }

  }catch(e){
      tf = '0';
      console.log(" Query error: "+e);
  }finally {
      if (connection && connection.end) connection.end();

      return tf;
  }

}

yesorno = await connectToDb();

  console.log('wat '+yesorno);
  res.send(yesorno);
});

//populate file table
//FIX THE DUPLICATES, YOU CAN ENTER THE SAME FILES LIKE AS MANY TIMES AS YOU WANT
app.get('/popFileTable',async function(req, res){
  let string = req.query.filename;
  //console.log(string);
  let gpxJSON = sharedLib.fileLogWrapper('uploads/'+string);
  let obj = JSON.parse(gpxJSON);
  let insFile = "INSERT INTO FILE (file_name, ver, creator) VALUES ('"+string+"',"+obj.version+",'"+obj.creator+"')";

  //insert data into file table on sql
  async function populateFileTable() {
    // get the client
    const mysql = require('mysql2/promise');

    let connection;
    let tf = '1';

    try{
        // create the connection
        connection = await mysql.createConnection(dbConf)

        // await connection.execute("DELETE FROM POINT;");
        // await connection.execute("DELETE FROM ROUTE;");
        // await connection.execute("DELETE FROM FILE;");
        // await connection.execute("ALTER TABLE FILE AUTO_INCREMENT = 0;");

        await connection.execute(insFile);

    }catch(e){
        tf = '0';
        console.log(" Query error: "+e);
    }finally {
        if (connection && connection.end) connection.end();
        return tf;
    }
  }
  populateFileTable();

  res.send(obj);
});

app.get('/popRouteTable',async function(req, res){
  let string = req.query.filename;
  let gpxRoute = sharedLib.routeViewPanelWrapper('uploads/'+string);
  let obj = JSON.parse(gpxRoute);
  let gpxJSON = sharedLib.fileLogWrapper('uploads/'+string);
  let gpxobj = JSON.parse(gpxJSON);
  let insRoute;

  //insert data into file table on sql
  async function populateRouteTable() {
    // get the client
    const mysql = require('mysql2/promise');

    let connection;

    try{
    //  console.log('1 '+insRoute);
        // create the connection
        connection = await mysql.createConnection(dbConf)

      //  console.log(insRoute);
        await connection.execute(insRoute);

    }catch(e){
        console.log(" Query error: "+e);
    }finally {
        if (connection && connection.end) connection.end();
    }
  }
//  if(gpxobj.numRoutes > 0){
    for(var i = 0;i<obj.length;i++){
      if (obj[i].name == 'None') {
        insRoute = "INSERT INTO ROUTE (route_len,gpx_id) VALUES ("+obj[i].len+","+req.query.idNum+")";
      }
      else{
        insRoute = "INSERT INTO ROUTE (route_name, route_len,gpx_id) VALUES ('"+obj[i].name+"',"+obj[i].len+","+req.query.idNum+")";
      }
      await populateRouteTable();
    }
  //}


  res.send(obj);
});

let index = 1;
app.get('/popPointTable',async function(req, res){
  let string = req.query.filename;
  let rtePoint = sharedLib.pointTable('uploads/'+string);
  //console.log(rtePoint);
  let obj = JSON.parse(rtePoint);
  let insPoint;

  //insert data into file table on sql
  async function populatePointTable() {
  //  console.log(insPoint);
    // get the client
    const mysql = require('mysql2/promise');

    let connection;

    try{
        // create the connection
        connection = await mysql.createConnection(dbConf)

        await connection.execute(insPoint);

    }catch(e){
        console.log(" Query error: "+e);
    }finally {
        if (connection && connection.end) connection.end();
    }
  }


  for(let i in obj){
    for (var j = 0; j < obj[i].length; j++) {
      if (obj[i][j].name == 'None') {
        insPoint = "INSERT INTO POINT (point_index, latitude,longitude,route_id) VALUES ("+(j+1)+","+obj[i][j].lat+","+obj[i][j].lon+","+index+");";
      }
      else{
        insPoint = "INSERT INTO POINT (point_index, latitude,longitude,point_name,route_id) VALUES ("+(j+1)+","+obj[i][j].lat+","+obj[i][j].lon+",'"+obj[i][j].name+"',"+index+");";
      }
      await populatePointTable();
    }
    index++;
  }


  res.send(obj);
});



app.get('/clearTableData',async function(req, res){

  index = 1;

  async function deleteAllData() {
    // get the client
    const mysql = require('mysql2/promise');

    let connection;

    try{
        // create the connection
        connection = await mysql.createConnection(dbConf)

        await connection.execute("DELETE FROM POINT");
        await connection.execute("DELETE FROM ROUTE");
        await connection.execute("DELETE FROM FILE");
        await connection.execute("ALTER TABLE POINT AUTO_INCREMENT = 0;");
        await connection.execute("ALTER TABLE ROUTE AUTO_INCREMENT = 0;");
        await connection.execute("ALTER TABLE FILE AUTO_INCREMENT = 0;");



    }catch(e){
        console.log(" Query error: "+e);
    }finally {
        if (connection && connection.end) connection.end();
    }
  }

  await deleteAllData();

  res.send('Deleted');
});

app.get('/displayDatabase',async function(req, res){

  let string;

  async function displayData() {
    // get the client
    const mysql = require('mysql2/promise');

    let connection;
    let toReturn;
    //let tf = '1';

    try{
        // create the connection
        connection = await mysql.createConnection(dbConf)

        const [fileNum] = await connection.execute("SELECT COUNT(*) AS result FROM FILE");
        const [routeNum] = await connection.execute("SELECT COUNT(*) AS result FROM ROUTE");
        const [pointNum] = await connection.execute("SELECT COUNT(*) AS result FROM POINT");

        toReturn = 'Database has '+fileNum[0].result+' files, '+routeNum[0].result+' routes,and '+pointNum[0].result+' points';
        console.log(toReturn);
        return toReturn;

    }catch(e){
        //tf = '0';
        console.log(" Query error: "+e);
    }finally {
        if (connection && connection.end) connection.end();
        //return tf;
    }
  }

  string = await displayData();

  res.send(string);
});

app.get('/queryToRun',async function(req, res){

  let value;

  async function queries() {
    // get the client
    const mysql = require('mysql2/promise');

    let connection;


    let q1NameA = "select * from ROUTE ORDER BY route_name ASC;";
    let q1LenA = "select * from ROUTE ORDER BY route_len ASC;";
    let q1NameD = "select * from ROUTE ORDER BY route_name DESC;";
    let q1LenD = "select * from ROUTE ORDER BY route_len DESC;";
    let q2NameA = "select file_name,route_name,route_len from FILE,ROUTE where FILE.file_name = '"+req.query.filename+"' and FILE.gpx_id = ROUTE.gpx_id ORDER BY route_name ASC;";
    let q2LenA = "select file_name,route_name,route_len from FILE,ROUTE where FILE.file_name = '"+req.query.filename+"' and FILE.gpx_id = ROUTE.gpx_id ORDER BY route_len ASC;";
    let q2NameD = "select file_name,route_name,route_len from FILE,ROUTE where FILE.file_name = '"+req.query.filename+"' and FILE.gpx_id = ROUTE.gpx_id ORDER BY route_name DESC;";
    let q2LenD = "select file_name,route_name,route_len from FILE,ROUTE where FILE.file_name = '"+req.query.filename+"' and FILE.gpx_id = ROUTE.gpx_id ORDER BY route_len DESC;";
    let q3A = "select point_id,point_index,latitude,longitude,point_name,ROUTE.route_id from ROUTE,POINT where ROUTE.route_id = "+req.query.id+" and ROUTE.route_id = POINT.route_id ORDER BY point_index ASC;"
    let q3D = "select point_id,point_index,latitude,longitude,point_name,ROUTE.route_id from ROUTE,POINT where ROUTE.route_id = "+req.query.id+" and ROUTE.route_id = POINT.route_id ORDER BY point_index DESC;"
    let q4A = "select route_name,point_id,point_index,latitude,longitude,point_name,POINT.route_id from FILE,ROUTE,POINT where FILE.file_name = '"+req.query.filename+"' and FILE.gpx_id = ROUTE.gpx_id and ROUTE.route_id = POINT.route_id ORDER BY route_name ASC, POINT.point_index ASC;";
    let q4D = "select route_name,point_id,point_index,latitude,longitude,point_name,POINT.route_id from FILE,ROUTE,POINT where FILE.file_name = '"+req.query.filename+"' and FILE.gpx_id = ROUTE.gpx_id and ROUTE.route_id = POINT.route_id ORDER BY route_name DESC, POINT.point_index DESC;";

    try{
        // create the connection
        connection = await mysql.createConnection(dbConf)
      //  const [toReturn];
        if(req.query.query == 'Query 1'){
          if(req.query.sort == 'Name'){
            if(req.query.asc == 'Ascending'){
              const [toReturn] = await connection.execute(q1NameA);
              return toReturn;
            }
            else if(req.query.asc == 'Descending'){
              const [toReturn] = await connection.execute(q1NameD);
              return toReturn;
            }
          }
          else if(req.query.sort == 'Length'){
            if(req.query.asc == 'Ascending'){
              const [toReturn] = await connection.execute(q1LenA);
              return toReturn;
            }
            else if(req.query.asc == 'Descending'){
              const [toReturn] = await connection.execute(q1LenD);
              return toReturn;
            }
          }
        }
        if(req.query.query == 'Query 2'){
          if(req.query.sort == 'Route Name'){
            if(req.query.asc == 'Ascending'){
              const [toReturn] = await connection.execute(q2NameA);
              return toReturn;
            }
            else if(req.query.asc == 'Descending'){
              const [toReturn] = await connection.execute(q2NameD);
              return toReturn;
            }
          }
          else if(req.query.sort == 'Route Length'){
            if(req.query.asc == 'Ascending'){
              const [toReturn] = await connection.execute(q2LenA);
              return toReturn;
            }
            else if(req.query.asc == 'Descending'){
              const [toReturn] = await connection.execute(q2LenD);
              return toReturn;
            }
          }
        }
        if(req.query.query == 'Query 3'){
          if(req.query.asc == 'Ascending'){
            const [toReturn] = await connection.execute(q3A);
            return toReturn;
          }
          else if(req.query.asc == 'Descending'){
            const [toReturn] = await connection.execute(q3D);
            return toReturn;
          }
        }
        if(req.query.query == 'Query 4'){
          if(req.query.asc == 'Ascending'){
            const [toReturn] = await connection.execute(q4A);
            return toReturn;
          }
          else if(req.query.asc == 'Descending'){
            const [toReturn] = await connection.execute(q4D);
            return toReturn;
          }
        }




    }catch(e){
        console.log(" Query error: "+e);
    }finally {
        if (connection && connection.end) connection.end();
    }
  }

  value = await queries();

  res.send(value);
});



























app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
