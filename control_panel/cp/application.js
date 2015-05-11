	     function connect() {
	         var socket;
	         var host = "ws://192.168.1.125";
			 

	         try {			 
			 
	             var socket = new WebSocket(host);

	             $('#loading').css('visibility', 'visible');				 
				 
	             message('<p class="event">Socket Status: ' + socket.readyState);

	             socket.onopen = function () {
	                 $('#loading').css('visibility', 'hidden');
	                 message('<p class="event">Socket Status: ' + socket.readyState + ' (open)');
	             }

	             socket.onmessage = function (msg) {
					try {
						var obj = JSON.parse(msg.data);
						if (obj.src == "WifiDongle") {
						var signal = "Wifi Strength: " + obj.msg;
							if (obj.msg > 50){$("span").text(signal);}
							if (obj.msg > 45 && obj.msg < 50){$("span").text(signal);}
							if (obj.msg < 45){$("span").text(signal);}
						}
						
						else if(obj.tgt == "Console") {
							message('<p class="message"><<: ' + obj.msg);
						}
											
					else {message('<p class="message"><<: ' + msg.data);}
					}

					catch(exception) {
						$('#cconsole').append("EXCEPTION: " + msg + '</p>');
					}
					
	                 
	             }
				 
				 function changeText() {
					var signal = $('#wifi');
					signal.text('Good!');
				 }

	             socket.onclose = function () {
	                 $('#loading').css('visibility', 'hidden');
	                 message('<p class="event">Socket Status: ' + socket.readyState + ' (Closed)');
	             }

	         } catch (exception) {
	             message('<p>Error' + exception);
	         }

	         function send() {
	             var text = $('#context').val();
				 
				 if (text == "disconnect") {
					message('<p class="message"><<: ' + "Disconnecting...");
					socket.close();
				 }

	             if (text == "") {
	                 message('<p class="warning">Please enter a message');
	                 return;
	             }
	             try {
	                 socket.send(text);
	                 message('<p class="event">>: ' + text)

	             } catch (exception) {
	                 message('<p class="warning">');
	             }
	             $('#context').val("");
	         }

	         function send1(text) {
	             if (text == "") {
	                 message('<p class="warning">Please enter a message');
	                 return;
	             }
	             try {
	                 socket.send(text);
	                 message('<p class="event">>: ' + text);

	             } catch (exception) {
	                 message('<p class="warning">');
	             }
	             $('#context').val("");
	         }

			 
			 function forwardGo() {
				var obj = {"tgt" : "MotorController", "msg" : 127};
				var sending = JSON.stringify(obj);
				send1(sending);
			 }
			 
			 function forwardStop() {
				var obj = {"tgt" : "MotorController", "msg" : 0};
				var sending = JSON.stringify(obj);
				send1(sending);
			 }

			 function backGo() {
				var obj = {"tgt" : "MotorController", "msg" : -127};
				var sending = JSON.stringify(obj);
				send1(sending);
			 }
			 
			 function backStop() {
				var obj = {"tgt" : "MotorController", "msg" : 0};
				var sending = JSON.stringify(obj);
				send1(sending);
			 }
			 
	         function message(msg) {
					$('#cconsole').append(msg + '</p>');
					$('.mainconsole').scrollTop($('.mainconsole')[0].scrollHeight);
	         }

	         $('#context').keypress(function (event) {
	             if (event.keyCode == '13') {
	                 send();
	             }
	         });

	         $('#hi').click(function () {
	             send1("hi");
	         });

	         $('#name').click(function () {
	             send1("name");
	         });

	         $('#time').click(function () {
	             send1("time");
	         });
				
			 	var i=1; var j=1;
				$('b#fwd').click (function () {
					if (i%2!=0) {forwardGo(); i++;} 
					else {forwardStop(); i++;}
					$(this).toggleClass("down");
				});
				
				$('b#bck').click (function () {
					if (j%2!=0) {backGo(); j++;} 
					else {backStop(); j++;}
					$(this).toggleClass("down");
				});
				
				$('b#lft').click (function () {
					$(this).toggleClass("down");
				});
				
				$('b#rgt').click (function () {
					$(this).toggleClass("down");
				});

	     } //End connect  

	     $(document).ready(function () {
	         if (!("WebSocket" in window)) {
	             $('#cconsole, input, button, #examples').fadeOut("fast");
	             $('<p>Browser not supported. Please download Mozilla Firefox.</p>').appendTo('#cconsole');
	         } else {
	             //The user has WebSockets  


	             connect();


	         } //End else  

	     });