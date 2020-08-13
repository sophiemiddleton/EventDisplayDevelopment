
	template <typename T, typename U> void DataLists(T data, U projection, bool Redraw, bool show2D, TEveElementList *List3D, TEveElementList *List2D){	
		  if(data == 0 && Redraw){
		    if (List3D != 0){
		      List3D->DestroyElements();
		    }
		    if(show2D){
		    if (List2D != 0){
		      List2D->DestroyElements();
		    }
		    projection->fXYMgr->ImportElements(List2D, projection->fDetXYScene); 
		    projection->fRZMgr->ImportElements(List2D, projection->fDetRZScene);
		    }
		    gEve->AddElement(List3D);
		    gEve->Redraw3D(kTRUE); 
		  } 
		  if(data!=0){
		    if (List3D== 0) {
		      List3D = new TEveElementList("3D Data");
		      List3D->IncDenyDestroy();     
		    }
		    else {
		      List3D->DestroyElements();  
		    }
		    if (List2D== 0) {
		      List2D = new TEveElementList("2D Data");
		      List2D->IncDenyDestroy();     
		    }
		    else {
		      List2D->DestroyElements();  
		    }
	}
}
/*
	template <typename S, typename M> DrawData(bool firstloop, S dataevt, M projection, double time, bool show2D, TEveElementList *List3D, TEveElementList *List2D, std::string datatype){


		      TEveMu2eHit *teve_hit2D = new TEveMu2eHit(dataevt);
		      TEveMu2eHit *teve_hit3D = new TEveMu2eHit(dataevt);


		      string pos3D = "(" + to_string((double)pointInMu2e.x()) + ", " + to_string((double)pointInMu2e.y()) + ", " + to_string((double)pointInMu2e.z()) + ")";
		      string pos2D = "(" + to_string((double)Pos.x()) + ", " + to_string((double)Pos.y()) + ", " + to_string((double)Pos.z()) + ")";
		      if (time == -1 || (datatime <= time && time != -1)){
			teve_hit3D->DrawHit3D(datatype + "3D, Position = " + pos3D + ", Energy = " + energy + ", Time = " + to_string(*datatime) + ", ", i + 1,  pointInMu2e, 2, tempList3D);
			
			List3D->AddElement(tempList3D); 
			if(show2D){
			teve_hit2D->DrawHit2D(datatype + "2D, Position = " + pos2D + ", Energy = " + energy + ", Time = " + to_string(*datatime) + ", ", i + 1, Pos, 2, tempList2D);
			List2D->AddElement(tempList2D); 
			// ... Import elements of the list into the projected views
			projection->fXYMgr->ImportElements(List2D, projection->fDetXYScene); 
			projection->fRZMgr->ImportElements(List2D, projection->fDetRZScene);
	}

*/

