function Graphics(t,i=512,e=384){this.container=document.getElementById(t),this._canvas=document.createElement("canvas"),this.container.appendChild(this._canvas),this._canvas.width=i,this._canvas.height=e,this._ctx=this._canvas.getContext("2d"),this.width=this._canvas.width,this.height=this._canvas.height,this.lines=function(t,i,e,s,a,h,n,l,r,o=0,u="black"){var g;if(o<0?(g=-o*(a+h),o=0):(g=o,g=((o=~~(o+.5))-g)*(a+h)),!(g>=e||o>=n.length)){var c=~~((e-g)/(a+h));c=c<n.length-o?c:n.length-o;var v=s/(r-l);this._ctx.save(),this._ctx.strokeStyle=u,this._ctx.beginPath();var f=.5+~~(t+g),m=i+s-v*(n[o].value-l);this._ctx.moveTo(f,.5+~~m),1==n[o].flag?this._ctx.lineTo(f+(a+h),.5+~~m):this._ctx.moveTo(f+(a+h),.5+~~m);for(var d=1;d<c;d++)f=.5+~~(t+g+d*(a+h)),m=i+s-v*(n[o+d].value-l),1==n[o+d].flag&&(1==n[o+d-1].flag?this._ctx.lineTo(f+(a+h),.5+~~m):(this._ctx.moveTo(f,.5+~~m),this._ctx.lineTo(f+(a+h),.5+~~m)));this._ctx.stroke(),this._ctx.restore()}},this.stepped=function(t,i,e,s,a,h,n,l,r,o=0,u="black"){var g;if(o<0?(g=-o*(a+h),o=0):(g=o,g=((o=~~(o+.5))-g)*(a+h)),!(g>=e||o>=n.length)){var c=~~((e-g)/(a+h));c=c<n.length-o?c:n.length-o;var v=s/(r-l);this._ctx.save(),this._ctx.strokeStyle=u,this._ctx.beginPath();var f=.5+~~(t+g),m=i+s-v*(n[o].value-l);this._ctx.moveTo(f,.5+~~m),1==n[o].flag?this._ctx.lineTo(f+(a+h),.5+~~m):this._ctx.moveTo(f+(a+h),.5+~~m);for(var d=1;d<c;d++)f=.5+~~(t+g+d*(a+h)),m=i+s-v*(n[o+d].value-l),1==n[o+d].flag&&(1==n[o+d-1].flag?(this._ctx.lineTo(f,.5+~~m),this._ctx.lineTo(f+(a+h),.5+~~m)):(this._ctx.moveTo(f,.5+~~m),this._ctx.lineTo(f+(a+h),.5+~~m)));this._ctx.stroke(),this._ctx.restore()}},this.ohlcbars=function(t,i,e,s,a,h,n,l,r,o=0,u="black"){var g;if(o<0?(g=-o*(a+h),o=0):(g=o,g=((o=~~(o+.5))-g)*(a+h)),!(g>=e||o>=n.length)){var c=~~((e-g)/(a+h));c=c<n.length-o?c:n.length-o-1;var v=s/(r-l);this._ctx.save(),this._ctx.beginPath(),this._ctx.strokeStyle="green";for(var f=.5+~~(t+g),m=0;m<=c;m++,f+=a+h){var d=i+s-v*(n[o+m].open-l),x=i+s-v*(n[o+m].high-l),p=i+s-v*(n[o+m].low-l),_=i+s-v*(n[o+m].close-l);1==n[o+m].flag&&_<=d&&(this._ctx.moveTo(f,.5+~~_),this._ctx.lineTo(f-(a+h>>1),.5+~~_),this._ctx.moveTo(f-(a+h>>1),.5+~~d),this._ctx.lineTo(f-(a+h),.5+~~d),this._ctx.moveTo(f-(a+h>>1),.5+~~x),this._ctx.lineTo(f-(a+h>>1),.5+~~p))}this._ctx.stroke(),this._ctx.beginPath(),this._ctx.strokeStyle="red",f=.5+~~(t+g);for(m=0;m<=c;m++,f+=a+h){d=i+s-v*(n[o+m].open-l),x=i+s-v*(n[o+m].high-l),p=i+s-v*(n[o+m].low-l),_=i+s-v*(n[o+m].close-l);1==n[o+m].flag&&_>d&&(this._ctx.moveTo(f,.5+~~_),this._ctx.lineTo(f-(a+h>>1),.5+~~_),this._ctx.moveTo(f-(a+h>>1),.5+~~d),this._ctx.lineTo(f-(a+h),.5+~~d),this._ctx.moveTo(f-(a+h>>1),.5+~~x),this._ctx.lineTo(f-(a+h>>1),.5+~~p))}this._ctx.stroke(),this._ctx.restore()}},this.y_axis=function(t,i,e,s,a,h,n=20,l,r="gray"){var o=(h-a)/(s/n),u=Math.log(o)/Math.log(10);u=u<0?~~(u-1):~~(u+.5);var g=Math.pow(10,u),c=o/g;c<=1?c=1:c<=1.25?(c=1.25,u-=2):c<=2.5?(c=2.5,u--):c<=2?c=2:c<=5?c=5:(c=10,u++),u=u<0?-u:0;var v=g*c,f=v*~~(a/v+1),m=s/(h-a);this._ctx.save(),this._ctx.strokeStyle=l,this._ctx.lineWidth=1,this._ctx.fillStyle=r,this._ctx.font="10px verdana",this._ctx.textAlign="right",this._ctx.textBaseline="bottom";for(var d=f;d<=h;d+=v){var x=.5+~~(i+s-m*(d-a));this._ctx.beginPath(),this._ctx.moveTo(t,x),this._ctx.lineTo(t+e,x),this._ctx.stroke(),this._ctx.fillText(d.toFixed(u),t+e-2,x)}this._ctx.restore()},this.timezone=3,this.timeunits=[{value:1,qt:1,unit:"s",shift:3600*this.timezone},{value:2,qt:1,unit:"s",shift:3600*this.timezone},{value:5,qt:1,unit:"s",shift:3600*this.timezone},{value:10,qt:1,unit:"s",shift:3600*this.timezone},{value:20,qt:1,unit:"s",shift:3600*this.timezone},{value:30,qt:1,unit:"s",shift:3600*this.timezone},{value:60,qt:60,unit:"m",shift:3600*this.timezone},{value:120,qt:60,unit:"m",shift:3600*this.timezone},{value:300,qt:60,unit:"m",shift:3600*this.timezone},{value:600,qt:60,unit:"m",shift:3600*this.timezone},{value:1200,qt:60,unit:"m",shift:3600*this.timezone},{value:1800,qt:60,unit:"m",shift:3600*this.timezone},{value:3600,qt:3600,unit:"h",shift:3600*this.timezone},{value:7200,qt:3600,unit:"h",shift:3600*this.timezone},{value:10800,qt:3600,unit:"h",shift:3600*this.timezone},{value:21600,qt:3600,unit:"h",shift:3600*this.timezone},{value:43200,qt:3600,unit:"h",shift:3600*this.timezone},{value:86400,qt:86400,unit:"d",shift:3600*this.timezone},{value:172800,qt:86400,unit:"d",shift:3600*this.timezone},{value:604800,qt:604800,unit:"w",shift:259200+3600*this.timezone},{value:1209600,qt:604800,unit:"w",shift:259200+3600*this.timezone},{value:2419200,qt:604800,unit:"w",shift:259200+3600*this.timezone}],this.time_grid=function(t,i,e,s,a,h,n=50,l="gray",r="gray"){var o,u=n*(h-a)/e;for(o=0;o<this.timeunits.length&&u>this.timeunits[o].value;o++);o=0==o?0:o-1;var g=this.timeunits[o].value,c=~~((a+this.timeunits[o].shift)/g+1)*g-this.timeunits[o].shift,v=e/(h-a);this._ctx.save(),this._ctx.strokeStyle=l,this._ctx.lineWidth=1,this._ctx.fillStyle=r,this._ctx.font="10px verdana",this._ctx.textBaseline="top";var f=new Date(1e3*(a+3600*this.timezone)),m=f.getUTCHours(),d=f.getUTCDate(),x=f.getUTCMonth()+1,p=f.getUTCFullYear();if("d"==this.timeunits[o].unit||"w"==this.timeunits[o].unit)for(var _=new Date(1e3*(a+3600*this.timezone)),T=0,z=a;z<h;T++){var b=_.getUTCMonth()+1,w=_.getUTCFullYear();_.setUTCHours(0),_.setMinutes(0),_.setSeconds(0),_.setUTCDate(1),_.setUTCMonth(b),b=_.getUTCMonth()+1,w=_.getUTCFullYear();var y=.5+~~(t+((z=_.getTime()/1e3-3600*this.timezone)-a)*v);0==T&&y-t>60&&this._ctx.fillText(p+"."+(x<10?"0":"")+x+"."+(d<10?"0":"")+d,0,0);var U=w+"."+(b<10?"0":"")+b;this._ctx.beginPath(),this._ctx.moveTo(y,i),this._ctx.lineTo(y,i+12),this._ctx.stroke(),this._ctx.fillText(U,y,0)}else if("h"==this.timeunits[o].unit)for(T=0,z=a;z<h;z+=86400,T++){0==T&&(z=86400*~~((a+this.timeunits[o].shift)/86400+1)-this.timeunits[o].shift);y=.5+~~(t+(z-a)*v);0==T&&y-t>60&&this._ctx.fillText(p+"."+(x<10?"0":"")+x+"."+(d<10?"0":"")+d,0,0);var C=(_=new Date(1e3*(z+3600*this.timezone))).getUTCDate();b=_.getUTCMonth()+1,U=(w=_.getUTCFullYear())+"."+(b<10?"0":"")+b+"."+(C<10?"0":"")+C;this._ctx.beginPath(),this._ctx.moveTo(y,i),this._ctx.lineTo(y,i+12),this._ctx.stroke(),this._ctx.fillText(U,y,0)}else if("m"==this.timeunits[o].unit)for(T=0,z=a;z<h;z+=3600,T++){0==T&&(z=3600*~~((a+this.timeunits[o].shift)/3600+1)-this.timeunits[o].shift);y=.5+~~(t+(z-a)*v);0==T&&y-t>95&&this._ctx.fillText(p+"."+(x<10?"0":"")+x+"."+(d<10?"0":"")+d+" "+(m<10?"0":"")+m+":00",0,0);var M=(_=new Date(1e3*(z+3600*this.timezone))).getUTCHours();C=_.getUTCDate(),b=_.getUTCMonth()+1,w=_.getUTCFullYear(),U=(b<10?"0":"")+b+"."+(C<10?"0":"")+C+" "+(M<10?"0":"")+M+"h";this._ctx.beginPath(),this._ctx.moveTo(y,i),this._ctx.lineTo(y,i+12),this._ctx.stroke(),this._ctx.fillText(U,y,0)}for(var D=c;D<h;D+=g){y=.5+~~(t+(D-a)*v);this._ctx.beginPath(),this._ctx.moveTo(y,i+12),this._ctx.lineTo(y,i+s),this._ctx.stroke();_=new Date(1e3*(D+3600*this.timezone));if("s"==this.timeunits[o].unit){var q=_.getUTCSeconds();U=(q<10?"0":"")+q+"''";this._ctx.fillText(U,y,12)}else if("m"==this.timeunits[o].unit){U=((b=_.getUTCMinutes())<10?"0":"")+b+"'";this._ctx.fillText(U,y,12)}else if("h"==this.timeunits[o].unit){U=((M=_.getUTCHours())<10?"0":"")+M+"h";this._ctx.fillText(U,y,12)}else if("d"==this.timeunits[o].unit){U=((C=_.getUTCDate())<10?"0":"")+C;this._ctx.fillText(U,y,12)}else if("w"==this.timeunits[o].unit){C=_.getUTCDate(),b=_.getUTCMonth()+1,U=(C<10?"0":"")+C;this._ctx.fillText(U,y,12)}}this._ctx.restore()},this.clear=function(){this._ctx.clearRect(0,0,this.width,this.height)}}class TimeInterval{constructor(t,i,e=function(){},s=function(){}){this.begin=t,this.end=i,this.onshift=e,this.onzoom=s}shift(t){this.begin+=t,this.end+=t,this.onshift()}zoom(t,i=0){if(!(t*(this.end-this.begin)<300)){var e=this.begin+i*(this.end-this.begin);this.begin=e-t*(e-this.begin),this.end=e+t*(this.end-e),this.onzoom()}}}class IrregularFloatDataset{constructor(t,i="stepped"){this.time=t,this.zmode=i,this.data=[],this.flag=0,this.value=0,this.high=Number.MIN_VALUE,this.low=Number.MAX_VALUE,this.open=0,this.close=0,this.topen=0,this.tclose=0,this.nclose=0,this.tstep=0,this.zdata=[]}push(t){if(!(t.time<this.time))if(0==this.data.length)this.flag=1,this.value=this.low=this.high=this.open=this.close=t.value,this.topen=this.tclose=t.time,this.data.push({flag:0,time:t.time,value:t.value});else{if(this.tclose>this.topen&&(this.value=(this.value*(this.tclose-this.topen)+(this.close-this.open)*(t.time-this.tclose))/(t.time-this.topen)),this.close==t.value&&1==t.flag)return void(0==this.nclose?(this.nclose++,this.tclose=t.time,this.data.push({flag:t.flag,time:t.time,value:t.value})):(this.data[this.data.length-1].time=t.time,this.tclose=t.time));this.low=Math.min(this.low,t.value),this.high=Math.max(this.high,t.value),this.close=t.value,this.tclose=t.time,this.nclose=0,this.data.push({flag:t.flag,time:t.time,value:t.value})}}getlinearregdata(t){for(var i=[],e=this.data[0],s=this.time,a=e.value;s+t<e.time;)i.push({flag:0,value:a}),s+=t;i.push({flag:1,value:a});for(var h=1;h<this.data.length;h++){e=this.data[h];var n=i.length-1,l=this.data[h-1];if(e.time<s+t)i[n].value+=(e.value-a)*(s+t-e.time)/t,a=e.value;else{s+=t;var r=(e.value-l.value)/(e.time-l.time);for(a=l.value+r*(s-l.time);s+t<e.time;){var o=a+r*t;i.push({flag:e.flag,value:(a+o)/2}),a=o,s+=t}i.push({flag:1,value:a})}}return i}getsteppedregdata(t){for(var i=[],e=this.data[0],s=this.time,a=e.value;s+t<e.time;)i.push({flag:0,value:a}),s+=t;i.push({flag:1,value:a});for(var h=1;h<this.data.length;h++){e=this.data[h];var n=i.length-1,l=this.data[h-1];if(e.time<s+t)i[n].value+=(e.value-a)*(s+t-e.time)/t,a=e.value;else{for(s+=t,a=l.value;s+t<e.time;)i.push({flag:e.flag,value:a}),s+=t;0==e.flag&&(a=e.value),i.push({flag:1,value:a})}}return i.push({flag:1,value:a}),i}rezoom(t){t<1&&(t=1),this.tstep!=t&&0!=this.data.length&&(this.tstep=t,"linear"==this.zmode?this.zdata=this.getlinearregdata(t):"stepped"==this.zmode&&(this.zdata=this.getsteppedregdata(t)))}fillzdata(t,i,e){if(this.tclose<=t.begin||this.topen>=t.end)return e;this.rezoom(i);var s=t.begin,a=~~((t.begin-this.time)/i),h=0;for(a<0&&(s=this.time,h=-a,a=0);s<t.end&&s<this.tclose;a++,h++,s+=i){var n=this.zdata[a].flag;e.zdata[h].flag=n;var l=this.zdata[a].value;e.zdata[h].value=l}return{zdata:e.zdata,min:Math.min(e.min,this.low),max:Math.max(e.max,this.high)}}integrate(t){var i=0;if(this.data.length>0)for(var e=this.data[0],s=1;s<this.data.length;s++){var a=this.data[s];a.time>t.begin&&a.time<=t.end&&(i+=e.value*(a.time-e.time),e=a)}return i}clearzdata(){this.zdata=[],this.tstep=0}}class TimeSlots{constructor(t,i){this.url=t,this.ext=i,this.array=[],this.parse=function(t,i,e){return{time:t,status:0}},this.onload=function(){},this.ondock=function(t,i){}}load(t,i,e="json"){if(!TimeSlots.loading&&void 0===this.array[t]){var s=86400*~~((Date.now()/1e3+10800)/86400)-10800;if(!(t>s)){TimeSlots.loading=!0;var a=new Date(1e3*(t+10800)),h=a.getUTCDate(),n=a.getUTCMonth()+1,l=a.getUTCFullYear(),r=this.url+l+(n<10?"0":"")+n+(h<10?"0":"")+h+"."+this.ext,o=new XMLHttpRequest;o.responseType=e,o.open("GET",r,!0),o.onreadystatechange=function(e){4==o.readyState&&(200==o.status?(this.array[t]=this.parse(t,o.response,i),void 0!==this.array[t+86400]&&0!=this.array[t+86400].flag&&this.ondock(this.array[t],this.array[t+86400]),void 0!==this.array[t-86400]&&0!=this.array[t-86400].flag&&this.ondock(this.array[t-86400],this.array[t])):this.array[t]={flag:0},TimeSlots.loading=!1,this.onload())}.bind(this),o.send(null),console.log(r)}}}preparezdata(t,i){for(var e=Number.MAX_VALUE,s=Number.MIN_VALUE,a=[],h=t.begin;h<t.end;h+=i)a.push({flag:0});return{zdata:a,min:e,max:s}}getzdata(t,i){for(var e=preparezdata(t,i),s=86400*~~((t.begin+10800)/86400)-10800;s<t.end;s+=86400)void 0===this.array[s]?this.load(s,i):0!=this.array[s].flag&&(e=this.array[s].fillzdata(t,i,e));return e}}TimeSlots.loading=!1;class ArduinoZone extends TimeSlots{constructor(t,i,e,s,a){super(t,"Z"+i),this.tcolor=e,this.hcolor=s,this.pcolor=a,this.showpower=!1,this.parse=function(t,i,e){var s=new IrregularFloatDataset(t),a=new IrregularFloatDataset(t),h=new IrregularFloatDataset(t);if(i){for(var n=i.split("\n"),l=0,r=(t=0,0),o=0,u=0,g=0,c=0,v=0,f=0;f<n.length;f++){var m=n[f].split(";");if(!(m.length<2))if(128&m[0]){t=0,r=0,o=0,u=0,c=0,l=parseInt(m[1]);var d=2;1&m[0]&&(t=parseFloat(m[d++])),2&m[0]&&(r=parseFloat(m[d++])),g=4&m[0]?100:0,h.push({flag:1,time:l,value:g}),16&m[0]&&(o=parseFloat(m[d++])),32&m[0]&&(u=parseFloat(m[d++])),64&m[0]&&(c=parseFloat(m[d++])),0==c&&(s.push({flag:0,time:l,value:t/10}),a.push({flag:0,time:l,value:r/10}))}else{if(0==l)continue;var x=parseInt(m[1]);(x=x>2147483647?x-4294967296:x)<0&&console.log(x,l,t,r,o,u,c),l+=x;d=2;1&m[0]&&(t+=parseFloat(m[d++])),2&m[0]&&(r+=parseFloat(m[d++])),4&m[0]?100!=g&&(g=100,h.push({flag:1,time:l,value:g})):0!=g&&(g=0,h.push({flag:1,time:l,value:g})),16&m[0]&&(o+=parseFloat(m[d++])),32&m[0]&&(u+=parseFloat(m[d++])),64&m[0]&&(c=parseFloat(m[d++])),v=0==c?1:0,0==c&&(1&m[0]&&s.push({flag:v,time:l,value:t/10}),2&m[0]&&a.push({flag:v,time:l,value:r/10}))}}0!=s.data.length&&s.push({flag:v,time:l,value:t/10}),0!=a.data.length&&a.push({flag:v,time:l,value:r/10}),0!=h.data.length&&h.push({flag:1,time:l,value:g})}return{t:s,h:a,p:h}}}preparezdata(t,i){for(var e={zdata:[],min:Number.MAX_VALUE,max:Number.MIN_VALUE},s={zdata:[],min:Number.MAX_VALUE,max:Number.MIN_VALUE},a={zdata:[],min:Number.MAX_VALUE,max:Number.MIN_VALUE},h=t.begin;h<t.end;h+=i)e.zdata.push({flag:0}),s.zdata.push({flag:0}),a.zdata.push({flag:0});return{t:e,h:s,p:a}}getzdata(t,i){for(var e=this.preparezdata(t,i),s=86400*~~((t.begin+10800)/86400)-10800;s<t.end;s+=86400)void 0===this.array[s]?this.load(s,i,"text"):0!=this.array[s].flag&&(e.t=this.array[s].t.fillzdata(t,i,e.t),e.h=this.array[s].h.fillzdata(t,i,e.h),e.p=this.array[s].p.fillzdata(t,i,e.p));return e}}class Diagram{constructor(t,i,e,s,a=0,h=40,n=0,l=2){this.width=i-a-h,this.height=e-25-0,this.marginleft=a,this.marginright=h,this.barw=n,this.barp=l,this.t=s,this.tstep=(this.t.end-this.t.begin)/this.width*(this.barw+this.barp),this.graph=new Graphics(t,i,e),this.graph._canvas.addEventListener("mousewheel",function(t){t.preventDefault(),t=t||window.event;var i=Math.pow(.9,t.detail?t.detail>0?-1:1:t.wheelDelta>0?1:-1),e=(t.offsetX-this.marginleft)/this.width;this.t.zoom(i,e)}.bind(this),!1),this.isDragging=!1,this.clientX0=0,this.graph._canvas.addEventListener("mousedown",function(t){t.preventDefault(),this.isDragging=!0,this.clientX0=t.offsetX}.bind(this),!1),this.graph._canvas.addEventListener("mousemove",function(t){if(t.preventDefault(),this.isDragging){var i=(t.offsetX-this.clientX0)/(this.barw+this.barp);this.clientX0=t.offsetX,this.t.shift(-i*this.tstep)}}.bind(this),!1),this.graph._canvas.addEventListener("mouseup",function(t){t.preventDefault(),this.isDragging=!1}.bind(this),!1)}rezoom(){this.tstep=(this.t.end-this.t.begin)/this.width*(this.barw+this.barp)}redraw(){this.tstep=(this.t.end-this.t.begin)/this.width*(this.barw+this.barp),this.graph.clear(),this.graph.time_grid(0,0,this.width,this.height+25,this.t.begin,this.t.end,60,"#666666"),this.graph._ctx.save(),this.graph._ctx.fillStyle="grey",this.graph._ctx.font="10px verdana",this.graph._ctx.textBaseline="bottom",this.graph._ctx.restore()}}