// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example

    $.ajax({
      type:'get',
      dataType:'json',
      url:'/serverFiles',
      async: false,
      timeout: 30000,
      success: function(data){
        for (var i = 0; i < data.length; i++) {
          console.log(data.length);
          let filename = data[i]

          $.ajax({//check valid and unlink if invalid
            type:'get',
            dataType:'text',
            url:'/checkValid',
            data: {
              filename: filename
            },
            success: function(returned){
              console.log(returned);
            },
            fail: function(error){
              console.log(returned);
              alert(error);
            }
          });
        }
      },
      fail:function(error){
        alert(error);
      }
    });


var buttonList=[];
var renameList = [];
var buttonList2=[];
var renameList2=[];
    $.ajax({
      type:'get',
      dataType:'json',
      url:'/serverFiles',
      async: false,
	    success: function(data){
    	   if(data.length != 0){
      		for (var i = 0; i < data.length; i++) {
            let fileNameStr = data[i];
            console.log(data[i]);
            $.ajax({
              type:'get',
              dataType:'json',
              url:'/gpxJSON',
              async: false,
              data: {
                filename: fileNameStr
              },
              //might have to like change the row link here and do the rest down theres
              success: function(data2){
                  //console.log(data2);
                  $('#fileLogPan').append('<tr><td>'+'<a href="/uploads/'+fileNameStr+'">'+fileNameStr+'</a></td><td>'+data2.version+'</td><td>'+data2.creator+'</td><td>'+data2.numWaypoints+'</td><td>'+data2.numRoutes+'</td><td>'+data2.numTracks+'</td></tr>');
                  $('#fileList').append('<option value='+fileNameStr+'>'+fileNameStr+'</option>');
                  $('#viewPanFileList').append('<option value='+fileNameStr+'>'+fileNameStr+'</option>');
                  $('#viewPanFileList2').append('<option value='+fileNameStr+'>'+fileNameStr+'</option>');
                  $('#queryFileList2').append('<option value='+fileNameStr+'>'+fileNameStr+'</option>');
              },
               fail: function(error){
                 alert(error);
               }
            });
          }
    	   }
    	   else{
            $('#mainRow').hide();
    		    $('#fileLogPan').append('<tr><td colspan = "6">No Files</td><tr>');
    	   }
          },
          fail: function(error){
            console.log("Recieved Error"+error);
          }
    });

    $("#viewPanFileListContainer").change(function(){//after change
      $('#gpxTable').empty();
      $('#gpxTable').append('<tr><th>Component</th><th>Name</th><th>Number of Points</th><th>Length</th><th>Loop</th>tr>');
      let viewString = $('#viewPanFileList').val();
      if(viewString){
          $.ajax({ //ajax call to get routeListToJSON and fill table
            type:'get',
            dataType:'json',
            async: false,
            url:'/gpxViewPanelRoute',
            data: {
              filename: viewString
            },
            success: function(data){
              for (var i = 0; i < data.length; i++) {
                $('#gpxTable').append('<tr><td>Route '+(i+1)+'<br><button id=route'+(i+1)+'>View Attributes</button><button id=renameRoute'+(i+1)+'>Rename</button></td><td>'+data[i].name+'</td><td>'+data[i].numPoints+'</td><td>'+data[i].len+'m</td><td>'+data[i].loop+'</td></tr>');
                buttonList[i] = 'route'+(i+1);
                renameList[i] = 'renameRoute'+(i+1);
              }
            },
            fail: function(error){
              alert(error);
            }
          });
          $.ajax({ //ajax call to get trackList toJson and fill table
            type:'get',
            dataType:'json',
            url:'/gpxViewPanelTrack',
            data: {
              filename: viewString
            },
            success: function(data){ //json.parse doesn't like \n
              for (var i = 0; i < data.length; i++) {
                $('#gpxTable').append('<tr><td>Track '+(i+1)+'<br><button id=track'+(i+1)+'>View Attributes</button><button id=renameTrack'+(i+1)+'>Rename</button></td><td>'+data[i].name+'</td><td></td><td>'+data[i].len+'m</td><td>'+data[i].loop+'</td></tr>');
                buttonList2[i] = 'track'+(i+1);
                renameList2[i] = 'renameTrack'+(i+1);
              }
            },
            fail: function(error){
              alert(error);
            }
          });
        }
      });

      $(document).on('click',function(e){
        for (var l = 0; l < renameList.length; l++) {
          if(e.target && e.target.id == renameList[l]){
            var num = l;
            let fileString = $('#viewPanFileList').val();
            let newName = $('#theName').val();
            if(!newName){
              alert('Enter a New Name');
            }
            else{
            $.ajax({
              type:'get',
              dataType:'text',
              url:'/gpxRenameRoute',
              data: {
                filename: fileString,
                name: newName,
                index: l
              },
              success: function(data){
                console.log(data);
              },
              fail: function(err){
                alert("Error from server: "+err);
              }
            });
            location.reload();
          }
          }
        }
      });
      $(document).on('click',function(e){
        for (var l = 0; l < renameList2.length; l++) {
          if(e.target && e.target.id == renameList2[l]){
            var num = l;
            let fileString = $('#viewPanFileList').val();
            let newName = $('#theName').val();
            if(!newName){
              alert('Enter a New Name');
            }
            else{
            $.ajax({
              type:'get',
              dataType:'text',
              url:'/gpxRenameTrack',
              data: {
                filename: fileString,
                name: newName,
                index: l
              },
              success: function(data){
                console.log(data);
              },
              fail: function(err){
                alert("Error from server: "+err);
              }
            });
            location.reload();
          }
          }
        }
      });
      $(document).on('click',function(e){
        for (var l = 0; l < buttonList.length; l++) {
          if(e.target && e.target.id == buttonList[l]){
            var num = l;
            let fileString = $('#viewPanFileList').val();
            console.log(fileString);
            $.ajax({
              type:'get',
              dataType:'json',
              url:'/gpxViewRouteAttr',
              data: {
                filename: fileString,
                num: l
              },
              success: function(data){ //this is whack stuff down below
                var bigStr = '';
                for (var i = 0; i < data.length; i++) {
                  if(typeof data[i].name !== 'undefined'){
                    bigStr = bigStr+' '+data[i].name+': '+data[i].value;
                    bigStr = bigStr+'\n';
                  }
                  else{
                    bigStr = 'No attributes for this Route';
                  }
                }

                // var newNum = 0;
                // var num2 = 0;
                // for (var k = 0; k < num; k++) {
                //   newNum = +newNum + +data[k].listLength;
                // }
                //
                // num2 = +newNum + +data[newNum].listLength;
                // for (let j = newNum; j < num2; j++) {
                //   bigStr = bigStr+' '+data[j].name+': '+data[j].value;
                //   bigStr = bigStr+'\n';
                // }
                // if(bigStr.length == 0){
                //   bigStr = "No Attributes for this Route";
                // }
                alert(bigStr);
              }
            });
          }
        }
      });
      $(document).on('click',function(e){
        for (var l = 0; l < buttonList2.length; l++) {
          if(e.target && e.target.id == buttonList2[l]){
            var num = l;
            let fileString = $('#viewPanFileList').val();
            console.log(fileString);
            $.ajax({
              type:'get',
              dataType:'json',
              url:'/gpxViewTrackAttr',
              data: {
                filename: fileString,
                num: l
              },
              success: function(data){ //this is whack stuff down below
                var bigStr = '';
                for (var i = 0; i < data.length; i++) {
                  if(typeof data[i].name !== 'undefined'){
                    bigStr = bigStr+' '+data[i].name+': '+data[i].value;
                    bigStr = bigStr+'\n';
                  }
                  else{
                    bigStr = 'No attributes for this Track';
                  }
              }
               alert(bigStr);
            },
            fail: function(err){
              alert('Recieved Error: '+err);
            }
            });
          }
        }
      });



      $('#fileForm').submit(function(submitE){
        let checkFile = $('#file').val();
        let checkFile2 = checkFile.split('.')

        console.log(checkFile2[1]);
        if(checkFile2[(checkFile2.length)-1] !== "gpx"){
          submitE.preventDefault();
          alert("Invalid file extension");
        }
      });


var mainArr = [];
      $('#findPath').click(function(){
        $('#gpxTable2').empty();
        $('#gpxTable2').append('<tr><th>Component</th><th>Name</th><th>Number of Points</th><th>Length</th><th>Loop</th>tr>');
        mainArr.length = 0;
        var latStart = $('#latS').val();
        var lonStart = $('#lonS').val();
        var latEnd = $('#latE').val();
        var lonEnd = $('#lonE').val();
        var deltaDelta = $('#delta').val();
        $.ajax({
          type:'get',
          dataType:'json',
          async: false,
          url:'/serverFiles',
          success: function(data){
            for (var i = 0; i < data.length; i++) {
              var filename = data[i]

              $.ajax({//check valid and unlink if invalid
                type:'get',
                dataType:'json',
                url:'/findRoutePath',
                async: false,
                data: {
                  filename: filename,
                  latStart: latStart,
                  lonStart: lonStart,
                  latEnd: latEnd,
                  lonEnd: lonEnd,
                  delta: deltaDelta
                },
                success: function(data){
                  mainArr = mainArr.concat(data);
                },
                fail: function(error){
                  alert(error);
                }
              });
              $.ajax({
                type:'get',
                dataType:'json',
                async: false,
                url:'/findTrackPath',
                data: {
                  filename: filename,
                  latStart: latStart,
                  lonStart: lonStart,
                  latEnd: latEnd,
                  lonEnd: lonEnd,
                  delta: deltaDelta
                },
                success: function(data){
                  mainArr = mainArr.concat(data);
                },
                fail: function(error){
                  alert(error);
                }
              });
            }
          },
          fail:function(error){
            alert('Recieved Error from server: '+error);
          }
        });
        mainArr.sort(function(a,b){
          return a.len - b.len;
        });
        var num = 0;
        var num2 =0;
        for (var l = 0; l < mainArr.length; l++) {
          if(typeof mainArr[l].numPoints === 'undefined'){
            $('#gpxTable2').append('<tr><td>Track Path '+(num+1)+'</td><td>'+mainArr[l].name+'</td><td></td><td>'+mainArr[l].len+'m</td><td>'+mainArr[l].loop+'</td></tr>');
            num++;
          }
          else{
            $('#gpxTable2').append('<tr><td>Route Path '+(num2+1)+'<br></td><td>'+mainArr[l].name+'</td><td>'+mainArr[l].numPoints+'</td><td>'+mainArr[l].len+'m</td><td>'+mainArr[l].loop+'</td></tr>');
            num2++;
          }
        }
      });


      $('#createGPXForm').submit(function(e){
        e.preventDefault();
        console.log('in here');
        let file = $('#fileName').val();
        let checkFile2 = file.split('.')
        let creator = $('#creator').val();
        let version = $('#version').val();

        if(checkFile2[(checkFile2.length)-1] !== "gpx"){
            alert("Invalid file extension");
        }
        $.ajax({
          type:'get',
          dataType:'text',
          url:'/writeNewGPX',
          data: {
            filename: file,
            creator: creator,
            version: version
          },
          success: function(data){
            console.log(data);
            //alert(data);
          },
          fail: function(error){
            alert(error);
          }
        });
        //location.reload();
       });

       $('#renameForm').submit(function(e){
         e.preventDefault();
         $.ajax({
           type:'get',
           dataType:'text',
           url:'/rename',
           success: function(data){
             alert('Recieved from server: '+data);
           },
           fail: function(err){
             alert('Recieved error: '+err);
             console.log(err);
           }
         });
       });

       var wpArr = [];
       var count = 0;
       $('#wpButton').click(function(){
         let lati = $('#lat').val();
         let long = $('#lon').val();

         wpArr[count] = lati;
         count++;
         wpArr[count] = long;
         count++;
         });

         $('#addRoute').click(function(){
           let fileStr = $('#fileList').val();
           for (var i = 0; i < wpArr.length; i++) {
             console.log(i);
             $.ajax({
               type:'get',
               dataType:'text',
               async: false,
               url:'/addRouteWrap',
               data: {
                 filename: fileStr,
                 lat: wpArr[i],
                 lon: wpArr[i+1],
                 value: i
               },
               success: function(data){
                 console.log('Route add was a '+data);
               },
               fail: function(err){
                 alert('Recieved error from server: '+err);
               }
             });
             i = +i+1;
           }
           wpArr.length = 0;
           location.reload();
           });


    $("#log").fadeOut('1');
    $("#gpx").fadeOut('1');

    $("#FileLog").click(function(){
      $("#log").slideToggle("slow");
    });
    $("#gpxView").click(function(){
      $("#gpx").slideToggle("slow");
    });
    $("#PathView").click(function(){
      $("#path").slideToggle("slow");
    });
    $("#panel").click(function(){
      $("#panelFlip").slideToggle("slow");
    });
    $("#Routepanel").click(function(){
      $("#panelFlipRoute").slideToggle("slow");
    });
    $("#betweenPanel").click(function(){
      $("#panelFlipBetween").slideToggle("slow");
    });
    $("#sqlTable1").click(function(){
      $("#table1Flip").slideToggle("slow");
    });
    $("#sqlTable2").click(function(){
      $("#table2Flip").slideToggle("slow");
    });
    $("#sqlTable3").click(function(){
      $("#table3Flip").slideToggle("slow");
    });
    $("#queryTable").click(function(){
      $("#table4Flip").slideToggle("slow");
    });


    $("#wpButton").click(function(){
      var lat = $("#lat").val();
      var lon = $("#lon").val();
      if (!lat){
        alert("Enter a Latitude");
      }
      else if(!lon){
        alert("Enter a Longitude");
      }
      else{
        console.log(lat,lon);
      }
    });


    //DataBase Stuff;
    $('#storeFiles').prop('disabled',true);
    $('#clearData').prop('disabled',true);
    $('#displayDB').prop('disabled',true);



    $("#connectBut").click(function(e){
      e.preventDefault();

      let usr = $('#dbuser').val();
      let pass = $('#dbpass').val();
      let name = $('#dbname').val();
      $.ajax({
        type:'get',
        dataType:'text',
        url:'/connectDB',
        data: {
          username: usr,
          passwrd: pass,
          dbname: name
        },
        success: function(data){
          console.log(data);
          if (data === '1') {
            alert('Welcome '+usr+', you\'re connected');
            $('#storeFiles').prop('disabled',false);
            $('#displayDB').prop('disabled',false);

          }
          else if(data == '0'){
            alert('Invalid Login\nPlease try again.')
          }
        },
        fail: function(err){
          alert('Recieved error from server: '+err);
        }
      });
    });


    $('#storeFiles').click(function(){
      $("#sqlFileTable").empty();
      $('#sqlFileTable').append('<tr id=\'sqlFileTableBody\'><th>Gpx ID</th><th>Filename</th><th>Version</th><th>Creator</th></tr>');
      $("#sqlRouteTable").empty();
      $('#sqlRouteTable').append('<tr id=\'sqlRouteTableBody\'><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>');
      $('#sqlPointTable').empty();
      $('#sqlPointTable').append('<tr id=\'sqlPointTableBody\'><th>point_id</th><th>point_index</th><th>latitude</th><th>longitude</th><th>point_name</th><th>route_id</th></tr>');
      $('#selectRoute2').empty();

      $('#viewQueryList2').prop('disabled',false);
      $('#aord2').prop('disabled',false);
      $.ajax({
        type:'get',
        dataType:'text',
        url:'/clearTableData',
        async: false,
        success: function(data){
          console.log(data);
        },
         fail: function(error){
           alert(error);
         }
      });
      var number = 1;
      var number2 = 1;
      var number3 = 1;
      var indexCheck;

      $.ajax({
        type:'get',
        dataType:'json',
        url:'/serverFiles',
        //async: false,
  	    success: function(data){
          let k = 1;
      	   if(data.length != 0){

        		for (var i = 0; i < data.length; i++) {
              let fileNameStr = data[i];
              $.ajax({
                type:'get',
                dataType:'json',
                url:'/popFileTable',
                async: false,
                data: {
                  filename: fileNameStr
                },
                success: function(data2){
                    $('#sqlFileTable').append('<tr><td>'+(number)+'</td><td>'+fileNameStr+'</td><td>'+data2.version+'</td><td>'+data2.creator+'</td>');
                    // number++;
                },
                 fail: function(error){
                   alert(error);
                 }
              });
              //call to populate route table
              $.ajax({
                type:'get',
                dataType:'json',
                url:'/popRouteTable',
                async: false,
                data: {
                  filename: fileNameStr,
                  idNum: number
                },
                success: function(data2){
                  for (var j = 0; j < data2.length; j++) {
                    $('#sqlRouteTable').append('<tr><td>'+(number2)+'</td><td>'+data2[j].name+'</td><td>'+data2[j].len+'</td><td>'+number+'</td>');
                    $('#selectRoute2').append('<option>'+number2+'</option>');
                    number2++;
                  }
                },
                 fail: function(error){
                   alert(error);
                 }
              });
              $.ajax({
                type:'get',
                dataType:'json',
                url:'/popPointTable',
                async: false,
                data: {
                  filename: fileNameStr,
                  index: number3
                },
                success: function(data2){

                  let l = 1;


                  for(let i in data2){
                    l = 1;
                    for (var j = 0; j < data2[i].length; j++) {
                      $('#sqlPointTable').append('<tr><td>'+k+'</td><td>'+l+'</td><td>'+data2[i][j].lat+'</td><td>'+data2[i][j].lon+'</td><td>'+data2[i][j].name+'</td><td>'+number3+'</td>');
                      k++;
                      l++;
                    }
                    number3++;
                  }
                },
                 fail: function(error){
                   alert(error);
                 }
              });
              number++;
            }
            $('#clearData').prop('disabled',false);
      	   }
      	   else{
              alert('No files on Server');
      	   }
            },
            fail: function(error){
              console.log("Recieved Error"+error);
            }
      });
    });

    $('#clearData').click(function(){
      $("#sqlFileTable").empty()
      $('#sqlFileTable').append('<tr id=\'sqlFileTableBody\'><th>Gpx ID</th><th>Filename</th><th>Version</th><th>Creator</th></tr>');
      $("#sqlRouteTable").empty();
      $('#sqlRouteTable').append('<tr id=\'sqlRouteTableBody\'><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>');
      $('#sqlPointTable').empty();
      $('#sqlPointTable').append('<tr id=\'sqlPointTableBody\'><th>point_id</th><th>point_index</th><th>latitude</th><th>longitude</th><th>route_id</th></tr>');
      $('#selectRoute2').empty();
      $.ajax({
        type:'get',
        dataType:'text',
        url:'/clearTableData',
        async: false,
        success: function(data){
          console.log(data);
        },
         fail: function(error){
           alert(error);
         }
      });
    });

    $('#displayDB').click(function(){
      $.ajax({
        type:'get',
        dataType:'text',
        url:'/displayDatabase',
        success: function(data){
          console.log(data);
          alert(data);
        },
         fail: function(error){
           alert(error);
         }
      });
    });

$('#viewQueryList2').prop('disabled',true);
$('#sortList2').prop('disabled',true);
$('#queryFileList2').prop('disabled',true);
$('#selectRoute2').prop('disabled',true);
$('#aord2').prop('disabled',true);

$("#viewQueryList").change(function(){
  let viewString = $('#viewQueryList2').val();
  $('#queryFileList2').prop('disabled',true);
  $('#sortList2').empty();
  if (viewString == 'Query 1') {
    $('#sortList2').append('<option value=\'Name\'>Name</option>');
    $('#sortList2').append('<option value=\'Length\'>Length</option>');
    $('#sortList2').prop('disabled',false);
    $('#aord2').prop('disabled',false);
    $('#selectRoute2').prop('disabled',true);
  }
  if (viewString == 'Query 2') {
    $('#sortList2').append('<option value=\'Route Name\'>Route Name</option>');
    $('#sortList2').append('<option value=\'Route Length\'>Route Length</option>');
    $('#queryFileList2').prop('disabled',false);
    $('#sortList2').prop('disabled',false);
    $('#aord2').prop('disabled',false);
    $('#selectRoute2').prop('disabled',true);
  }
  if (viewString == 'Query 3') {
    $('#selectRoute2').prop('disabled',false);
    $('#sortList2').prop('disabled',true);
    $('#aord2').prop('disabled',false);
  }
  if (viewString == 'Query 4') {
    $('#queryFileList2').prop('disabled',false);
    $('#sortList2').prop('disabled',true);
    $('#aord2').prop('disabled',false);
    $('#selectRoute2').prop('disabled',true);
  }
});

$('#executeQuery').click(function(){
  $("#executeQueryTable").empty()

  let queryPick = $('#viewQueryList2').val();
  let sortby = $('#sortList2').val();
  let file = $('#queryFileList2').val();
  let routeIdNum = $('#selectRoute2').val();
  console.log(routeIdNum );
  if(routeIdNum == null){
    routeIdNum = 0;
  }
  let updown = $('#aord2').val();

  $.ajax({
    type:'get',
    dataType:'text',
    url:'/queryToRun',
    data: {
      query: queryPick,
      sort: sortby,
      filename: file,
      id: routeIdNum,
      asc:updown
    },
    async: false,
    success: function(data){
      console.log(data);
      let obj = JSON.parse(data);
      if(queryPick == 'Query 1'){
        $('#executeQueryTable').append('<tr id=\'executeQueryTableBody\'><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>');
        for(var i = 0;i<obj.length;i++){
          if(obj[i].route_name == null){
            $('#executeQueryTable').append('<tr><td>'+obj[i].route_id+'</td><td>\'Empty\'</td><td>'+obj[i].route_len+'</td><td>'+obj[i].gpx_id+'</td>');
          }
          else{
            $('#executeQueryTable').append('<tr><td>'+obj[i].route_id+'</td><td>'+obj[i].route_name+'</td><td>'+obj[i].route_len+'</td><td>'+obj[i].gpx_id+'</td>');
          }
        }
        if (obj.length == 0) {
          $('#executeQueryTable').append('<tr><td colspan= "4">No results</td>');
        }
      }
      if(queryPick == 'Query 2'){
        $('#executeQueryTable').append('<tr id=\'executeQueryTableBody\'><th>File Name</th><th>route_name</th><th>route_len</th></tr>');
        for(var i = 0;i<obj.length;i++){
          if(obj[i].route_name == null){
            $('#executeQueryTable').append('<tr><td>'+file+'</td><td>\'Empty\'</td><td>'+obj[i].route_len+'</td>');
          }
          else{
            $('#executeQueryTable').append('<tr><td>'+file+'</td><td>'+obj[i].route_name+'</td><td>'+obj[i].route_len+'</td>');
          }
        }
        if (obj.length == 0) {
          $('#executeQueryTable').append('<tr><td colspan= "3">No results</td>');
        }
      }
      if(queryPick == 'Query 3'){
        $('#executeQueryTable').append('<tr id=\'executeQueryTableBody\'><th>point_id</th><th>point_index</th><th>latitdue</th><th>longitude</th><th>point_name</th><th>route_id</th></tr>');
        for(var i = 0;i<obj.length;i++){
          if(obj[i].point_name == null){
            $('#executeQueryTable').append('<tr><td>'+obj[i].point_id+'</td><td>'+obj[i].point_index+'</td><td>'+obj[i].latitude+'</td><td>'+obj[i].longitude+'</td><td>\'Empty\'</td><td>'+obj[i].route_id+'</td>');
          }
          else{
            $('#executeQueryTable').append('<tr><td>'+obj[i].point_id+'</td><td>'+obj[i].point_index+'</td><td>'+obj[i].latitude+'</td><td>'+obj[i].longitude+'</td><td>'+obj[i].point_name+'</td><td>'+obj[i].route_id+'</td>');
          }
        }
        if (obj.length == 0) {
          $('#executeQueryTable').append('<tr><td colspan= "6">No results</td>');
        }
      }
       if(queryPick == 'Query 4'){
        $('#executeQueryTable').append('<tr id=\'executeQueryTableBody\'><th>route_name</th><th>point_id</th><th>point_index</th><th>latitdue</th><th>longitude</th><th>route_id</th></tr>');
          for(var i = 0;i<obj.length;i++){
            if (obj[i].route_name == null) {
              $('#executeQueryTable').append('<tr><td>unnamed route '+(i+1)+'</td><td>'+obj[i].point_id+'</td><td>'+obj[i].point_index+'</td><td>'+obj[i].latitude+'</td><td>'+obj[i].longitude+'</td><td>'+obj[i].route_id+'</td>');
            }
            else{
            $('#executeQueryTable').append('<tr><td>'+obj[i].route_name+'</td><td>'+obj[i].point_id+'</td><td>'+obj[i].point_index+'</td><td>'+obj[i].latitude+'</td><td>'+obj[i].longitude+'</td><td>'+obj[i].route_id+'</td>');
            }
          }
          if (obj.length == 0) {
            $('#executeQueryTable').append('<tr><td colspan= "6">No results</td>');
          }
       }


    },
     fail: function(error){
       alert(error);
     }
  });
});


//for tomroow
// add a route selection list for query 3
// most important, make the c function to get points from each route and then fill the points table and sql table
// queries should be done but may need tweaking?















});
