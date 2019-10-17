#include <kernel/parallel/messagesink.h>


void MessageSink::CollectText(int when, String what){
	
	priv << what << endl;

};

int MessageSink::Flush(){

	if(Root)
		cout << all.str();


};

void MessageSink::Synchronize(){


// zmienic jakos na proc_size
	for (int i=0; i < Get_size_static(); i++){
		string s = OneToOne(i, 0, priv.str());
		if (Get_rank_static()==0){
			all << s;
		};
		

	};

};
