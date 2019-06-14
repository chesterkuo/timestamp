var express = require('express');
var router = express.Router();
var multer = require('multer');
var fs = require('fs');
var sha256 = require('sha256');
var request = require('request');

const UPLOAD_PATH = './uploads'

var upload = multer({ dest: UPLOAD_PATH })

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});


//多文件上传
//router.post('/upload', upload.array('fileUpload'), function (req, res, next) {
//  const files  = req.files;
//  const response = [];
//  const result = new Promise((resolve, reject) => {
//    files.map((v) => {
//      fs.readFile(v.path, function(err, data) {
//        fs.writeFile(`${UPLOAD_PATH}/${v.originalname}`, data, function(err, data) {
//          const result = {
//            file: v,
//          }
//          if (err)  reject(err);
//          resolve('成功');
//        })
//      })
//    })
//  })
//  result.then(r => {
//    res.json({
//      msg: '上传成功',
//    })
//  }).catch(err => {
//    res.json({ err })
//  });
//})

router.post('/upload', upload.single('fileUpload'), function (req, res, next) {
   const { file } = req;
   var sha = "";
   var my = {};
   fs.readFile(file.path, function(err, data) {
      //fs.writeFileSync(`${UPLOAD_PATH}/${file.originalname}`, data, function (err) {
       if (err) res.json({ err })
       sha = sha256(data);
       //res.json("SHA256 :" + sha);
       my["SHA256"] = sha256(data);
       my["File"] = file.originalname;
       my["TimeStamp"] = new Date;
       //console.log("output1:" + my["SHA256"]);
     //});
       var json_obj = JSON.stringify(my);
       var post_data = {
          "API_NAME": "ADD_DATA",
          "entity": "File",
          "jsons": json_obj
       };
       request.post('http://127.0.0.1:3333/v1/timestamp/add_data', {
         json: post_data,
         headers: {'Content-Type': 'application/json'}
       }, (error, myres, body) => {
         if (error) {
           console.error(error)
           return
	 //res.json("Done");
         }
	 //console.log(myres);
	 //console.log(body['transaction id']);
         var rt_body = body['transaction id'];
         res.json("transaction logged to EOS/BOS public chain : https://bos.bloks.io/transaction/" + rt_body);
       })
   })
   //res.json("transaction :" + rt_body);
   fs.unlinkSync(file.path); 
   //fs.deleteFile();
})

module.exports = router;
