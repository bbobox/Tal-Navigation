package extraction;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ConstructionListeEntite {
	
	private String fichierTag;
	private String listEntite;
	
	public ConstructionListeEntite(String fileTag, String fileSortie){
		fichierTag=fileTag;
		listEntite=fileSortie;
		
		
		BufferedReader Lecteur;// = null;
		//BufferedWriter ListeEntite;//=null;
		String ligne;
		ArrayList<String[]>  T= new ArrayList<String[]>();
		
		String Temp="";

				try {
					Lecteur=new BufferedReader(new  FileReader(fichierTag));
					BufferedWriter ListeEntite= new BufferedWriter(new FileWriter(listEntite));
					
					//ListeEntite.write("<enamex><person>Charles Baudelaire</person></enamex>");
					//Sequence.close();
					
					Pattern p = Pattern.compile("[{][S][}]");
					
					int i=0;
					int cpt=0;
					while(((ligne = Lecteur.readLine()) != null)){
						
						//System.out.println(ligne);
						String ligneTemp="";
						String[] temp=p.split(ligne);
						if (temp.length>1){
							ligneTemp=temp[0];
							String phrase=Temp+ligneTemp;
							Temp=temp[temp.length-1];
							//System.out.println(i);
							ArrayList<String> text= attributsEntite(phrase);
							String test=new String("");
							if(phrase.equals(test)==false){
								i++;
								
								for(int k=0;k<text.size();k++){
									cpt++;
									ListeEntite.write(text.get(k));ListeEntite.newLine();
									//System.out.println(text.get(k));
									
								}
						}
							for(int j=0;j<temp.length-1;j++){
								text= attributsEntite(temp[j]);
								test=new String("");
								if(temp[j].equals(test)==false){
									i++;
								
									for(int k=0;k<text.size();k++){
										cpt++;
										ListeEntite.write(text.get(k));ListeEntite.newLine();
										//System.out.println(text.get(k));
										}
									}
							}
							
						}
						else{
							Temp=Temp+ligne;
						}
						
					}	
					
					ArrayList<String> text= attributsEntite(Temp);
						String test=new String("");
						if(Temp.equals(test)==false){
							i++;
							
							for(int k=0;k<text.size();k++){
								ListeEntite.write(text.get(k));ListeEntite.newLine();
								//System.out.println(text.get(k));
							}
					
					}
					ListeEntite.close();
					
					/*String[] sort= {"sort","-o","listEntiteSort.txt","listEntite.txt"};
					 try {
						Process proc=Runtime.getRuntime().exec(sort);
					 } catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					 }
					 
					 String[] uniq= {"uniq","listEntiteSort.txt",listEntite};
					 try {
						Process proc=Runtime.getRuntime().exec(uniq);
					 } catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					 }	*/
					
					} catch (IOException e) {
					// TODO Auto-generated catch block
					System.err.println(e);
					e.printStackTrace();
					}
				/*
				finally
				{ 
				   try{
				      if(ListeEntite!=null)
				    	  ListeEntite.close();
				   }catch(Exception ex){
				       System.out.println("Error in closing the BufferedWriter"+ex);
				    }
				}
		*/
		
	}
	
	
	
	
	
	public ArrayList<String> attributsEntite(String Text){
		//Pattern p = Pattern.compile("<.*?><.*?>(.*?)</.*?></.*?>");
		Pattern p = Pattern.compile("<.*?>(<.*?>)+(.*?)</.*?>(</.*?>)+");
		Matcher m = p.matcher(Text);
		ArrayList<String> res=new ArrayList<String>();
		//System.out.println(">>>>>"+Text);
		while(m.find()){
		   // System.out.println(m.group(0)); 
		    res.add(m.group(0));
		
		}

		return res;
	}

	public static void main(String args[]){

		ConstructionListeEntite fichierListeEntite= new ConstructionListeEntite("abConv.tag.txt","iiiiiii.txt");
		
	}
	
}
