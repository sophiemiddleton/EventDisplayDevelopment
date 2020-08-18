
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

	template <typename K> int GetTime(K data, int type){
		switch(type){
			case 1:{
				return data.time();
				break; }
			case 2:{
				return data.GetPulseTime();
				break; }

		}
		return (0);
	}

	template <typename T> double GetEnergy(T data, int type){
		switch(type){
			case 1:{
				return data.energyDep();
				break; }
			/*case 2:
				return data.energy();
				break;
			case 3:
				return data.GetEnergy();
				break;*/
		}
		return (0.0);
	}


/*	template <typename L> void Energies(L data, int type, double *energies[], int *energylevels[]){

		double Max_Energy = 0;
		double Min_Energy = 1000;
		for(unsigned int i=0; i < data->size();i++){
		      if (GetEnergy<L>((*data)[i],type) > Max_Energy){Max_Energy = GetEnergy<L>((*data)[i],type);}
		      if (GetEnergy<L>((*data)[i],type)< Min_Energy){Min_Energy = GetEnergy<L>((*data)[i],type);}
		    }
		double interval = (Max_Energy - Min_Energy)/(12);


		for(size_t i=0; i<data->size();i++){
		for(size_t n=0; n<12;n++){
		     if(GetEnergy<L>((*data)[i],type) >= Min_Energy + n * interval && GetEnergy<L>((*data)[i],type) <=Min_Energy + (n+1)*interval){*energylevels[i] = n;}
		       }
		    }
		*energies[0] = Min_Energy;
		*energies[1] = Max_Energy;

	}
	template <typename T> void Test(void (*M)(), T hit){
		std:cout<<to_string(hit.M())<<std::endl;
	}
*/
	template <typename S, typename M, typename E> void DrawHitData(bool firstloop, S dataevt, double time, bool show2D, std::string datatype, int i, int type, TEveElementList **List3D, TEveElementList **tempList3D, E teve_data3D, E teve_data2D = 0, TEveElementList **List2D = 0, TEveElementList **tempList2D = 0, M projection = 0){
		      CLHEP::Hep3Vector pointInMu2e;

		      CLHEP::Hep3Vector DataPos(data->pos().x(), data->pos().y(), data->pos().z());;
		      CLHEP::Hep3Vector pointInMu2e = PointToTracker(DataPos);

		      double energy = GetEnergy<S>(dataevt, type);
		      string pos3D = "(" + to_string((double)DataPos.x()) + ", " + to_string((double)DataPos.y()) + ", " + to_string((double)DataPos.z()) + ")";
		      string pos2D = "(" + to_string((double)pointInMu2e.x()) + ", " + to_string((double)pointInMu2e.y()) + ", " + to_string((double)pointInMu2e.z()) + ")";

		      if (time == -1 || (dataevt.time() <= time && time != -1)){
			teve_data3D->DrawHit3D(datatype + "3D, Position = " + pos3D + ", Energy = " + to_string(energy) + ", Time = " + to_string(dataevt.time()) + ", ", i + 1,  pointInMu2e, 2, *tempList3D);
			(*List3D)->AddElement(*tempList3D); 


			if(show2D){
				if (type == 1){teve_data2D->DrawHit2D(datatype + "2D, Position = " + pos2D + ", Energy = " + to_string(energy) + ", Time = " + to_string(dataevt.time()) + ", ", i + 1, DataPos, 2, *tempList2D);}
				(*List2D)->AddElement(*tempList2D); 
				// ... Import elements of the list into the projected views
				projection->fXYMgr->ImportElements(*List2D, projection->fDetXYScene); 
				projection->fRZMgr->ImportElements(*List2D, projection->fDetRZScene);
			}
    }
}
