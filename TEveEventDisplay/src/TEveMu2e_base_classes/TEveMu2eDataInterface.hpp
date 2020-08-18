
    template <typename T, typename U> void DataLists(T data, bool Redraw, bool show2D, TEveElementList **List3D, TEveElementList **List2D = 0, U projection = 0){	
		  if(data == 0 && Redraw){
		    if (*List3D != 0){
		      (*List3D)->DestroyElements();
		    }
		    if(show2D){
		    if (*List2D != 0){
		      (*List2D)->DestroyElements();
		    }
		    projection->fXYMgr->ImportElements(*List2D, projection->fDetXYScene); 
		    projection->fRZMgr->ImportElements(*List2D, projection->fDetRZScene);
		    }
		    gEve->AddElement(*List3D);
		    gEve->Redraw3D(kTRUE); 
		  } 
		  if(data!=0){
		    if (*List3D== 0) {
		      *List3D = new TEveElementList("3D Data");
		      (*List3D)->IncDenyDestroy();     
		    }
		    else {
		      (*List3D)->DestroyElements();  
		    }
		    if (*List2D== 0) {
		      *List2D = new TEveElementList("2D Data");
		      (*List2D)->IncDenyDestroy();     
		    }
		    else {
		      (*List2D)->DestroyElements();  
		    }
	}
}


	template <typename L> std::vector<double> Energies(L data, int *energylevels[]){
		std::vector<double> energies = {0, 0};
		double Max_Energy = 0;
		double Min_Energy = 1000;
		for(unsigned int i=0; i < data->size();i++){
		      if (((*data)[i]).energyDep() > Max_Energy){Max_Energy = ((*data)[i]).energyDep();}
      		      if (((*data)[i]).energyDep()< Min_Energy){Min_Energy = ((*data)[i]).energyDep();}
		    }
		double interval = (Max_Energy - Min_Energy)/(12);


		for(size_t i=0; i<data->size();i++){
		for(int n=0; n<12;n++){
		     if(((*data)[i]).energyDep() >= Min_Energy + n * interval && ((*data)[i]).energyDep() <=Min_Energy + (n+1)*interval){*(energylevels[i]) = n;}
		       }
		    }
		energies.at(0) = Min_Energy;
		energies.at(1) = Max_Energy;
		return energies;
	}




