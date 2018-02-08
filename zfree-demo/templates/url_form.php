<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
<!-- Latest compiled and minified CSS -->
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">

<!-- Optional theme -->
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css" integrity="sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp" crossorigin="anonymous">

<!-- Latest compiled and minified JavaScript -->
<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js" integrity="sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa" crossorigin="anonymous"></script>

<style type="text/css">

html body {
  margin-top: 50px !important;
}

#top_form {
  position: fixed;
  top:0;
  left:0;
  width: 100%;
  
  margin:0;
  
  z-index: 2100000000;
  -moz-user-select: none; 
  -khtml-user-select: none; 
  -webkit-user-select: none; 
  -o-user-select: none; 
  
  border-bottom:1px solid #151515;
  
    background:#FFC8C8;
  
  height:45px;
  line-height:45px;
}

#top_form input[name=url] {
  width: 300px;
  height: 30px;
  padding: 5px;
  font: 13px "Helvetica Neue",Helvetica,Arial,sans-serif;
  border: 0px none;
  background: none repeat scroll 0% 0% #FFF;
}

.our-desc {
  color: yellow;
  font: 15px "Helvetica Neue",Helvetica,Arial,sans-serif;
}

.our-btn{
   all: initial;
  * {
    all: unset;
  }
}

.our-highlight{
  color: red;
}

</style>

<script>
var url_text_selected = false;

function smart_select(ele){

  ele.onblur = function(){
    url_text_selected = false;
  };
  
  ele.onclick = function(){
    if(url_text_selected == false){
      this.focus();
      this.select();
      url_text_selected = true;
    }
  };
}
</script>

<div id="top_form" style="height: 300px; opacity: 0.8; background: black;">

  <div style="width: 600px;
    margin-left: auto;
    margin-right: auto;">
    <div class="row">
      <div >
        <form id="proxy-form" method="post" action="index.php" target="_top" style="margin:0; padding:0;" >
          <input class="btn btn-default" type="button" value="Home" onclick="window.location.href='index.php'">
          <input id="input-url" type="text" name="url" value="<?php echo $url; ?>" autocomplete="off">
          <input type="hidden" name="form" value="1">
          <input class="btn btn-default" type="submit" value="Go">
        </form>
        <p class="our-desc">
          This is a demo website of our secure zero-rating framework (ZFree).  When you input a URL in the above URL form, 
		we will redirect your traffic through our ISP, i.e., a web proxy.  We create a demo billing system in our ISP. That is,
		 when you visit our uncharged websites, you will get free, uncharged traffic; otherwise, when you visit normal websites, 
		 all your traffic will be charged. 
        </p>
	      <button class="btn btn-default" type="button" id="visit-our-server">Visit Uncharged Website 1</button>
        <button class="btn btn-default" type="button" id="visit-our-server2">Visit Uncharged Website 2</button>
        <button class="btn btn-default" type="button" id="visit-google">Visit Charged Website (Google)</button>
      </div>
      <div >
        <p class="our-desc our-highlight">Uncharged Network Traffic: </p>
        <p id="uncharged-data" class="our-desc">Uncharge Amount</p>
        <p class="our-desc our-highlight">Charged Network Traffic: </p>
        <p id="charged-data" class="our-desc">Charge Amount</p>
      </div>
    </div>   
  </div>
  
</div>

<script type="text/javascript">
  smart_select(document.getElementsByName("url")[0]);
</script>

<script type="text/javascript">
  var base_url = "10.0.0.10";
  var rest_url = "10.0.0.11";

  $.ajax({
      type: "GET",
      url: "http://" +rest_url+":9080/packets",
      success: function(result){
        var text = sizeConvert(result);
        $("#uncharged-data").text(text);
      }
  });

  $.ajax({
      type: "GET",
      url: "http://" +rest_url+":9080/total",
      success: function(result){
        var text = sizeConvert(result);
        $("#charged-data").text(text);
      }
  });

  function sizeConvert(data){
    var rst = parseFloat(data) / 1024.0;
    if (rst < 1024) {
      return rst.toFixed(3) + "KB";
    }
    rst = rst / 1024.0;
    if (rst < 1024){
      return rst.toFixed(3) + "MB";
    }
    rst = rst / 1024.0;
    return rst.toFixed(3)  + "GB";
  }

  $("#visit-our-server").click(function(){
		$("#input-url").val(base_url + "/demo1");
		$("#proxy-form").submit();
	});

  $("#visit-our-server2").click(function(){
		$("#input-url").val(base_url +"/demo2");
		$("#proxy-form").submit();
	});
  
	$("#visit-google").click(function(){
		$("#input-url").val("www.google.com");
		$("#proxy-form").submit();
	});
</script>

