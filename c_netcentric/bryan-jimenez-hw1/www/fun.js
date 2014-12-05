function main(){
	var elements = document.getElementsByTagName('img');
	
	for(var i = 0; i < elements.length; i++) {

		 elements[i].setAttribute('onclick', 'alert(this.src);');
		 
	}
}
