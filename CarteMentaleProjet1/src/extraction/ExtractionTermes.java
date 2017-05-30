package extraction;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ExtractionTermes {
	private String termesYatea; // fichier contenant les termes extraits par Yatea
	private String termesRelationSemantique; // fichier cotennat les termes extraits des relations
	private String fichierSortie;
	
	public ExtractionTermes(String f1, String f2,String fSortie){
		termesYatea=f1;
		termesRelationSemantique=f2;
		fichierSortie=fSortie;
	
	}
	
	public void productionFichier(){
		
		 /* Recuperation de La Liste de termes Produite Par Yatea*/
		BufferedReader Lecteur = null;
		
		String ligne;
		String terme;
		BufferedWriter fSortie;
		try {
			fSortie = new BufferedWriter(new  FileWriter(fichierSortie));
		
		try {
			Lecteur=new BufferedReader(new  FileReader(termesYatea));
			//BufferedWriter fSortie=new BufferedWriter(new  FileWriter(fichierSortie));
			Pattern p = Pattern.compile("[\t]");
			
			while(((ligne = Lecteur.readLine()) != null)){
				//System.out.println(ligne);
					if(!ligne.startsWith("#")){
						terme= p.split(ligne)[0];
						fSortie.write(terme);fSortie.newLine();
						//System.out.println(terme);

				}
			}	
			
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	/* Recuperation des termmes sortie des relations */
		StringBuilder b;
		try {
			Lecteur=new BufferedReader(new  FileReader(termesRelationSemantique));
			Pattern p = Pattern.compile("<t1> (.*?)</t1>");
			Pattern p2 = Pattern.compile("<t2> (.*?)</t2>");
			while(((ligne = Lecteur.readLine()) != null)){
				Matcher m=p.matcher(ligne);
				
				if(m.find()){
					//System.out.println(m.group(1));
					fSortie.write(m.group(1));fSortie.newLine();
				}
				
				Matcher m2=p2.matcher(ligne);
				if(m2.find()){
					//System.out.println(m2.group(1));
					fSortie.write(m2.group(1));fSortie.newLine();
				}
				
				//System.out.println("lignnn"+ligne);
				
				
			}
			fSortie.close();
		} 
		catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
			
				
			
	
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		String[] sort= {"sort","-o",fichierSortie,fichierSortie};
		 try {
			Process proc=Runtime.getRuntime().exec(sort);
			//System.out.println("fait");
			//Runtime.getRuntime().exec("gedit text.Unitex_csc.txt");
			try {
				proc.waitFor();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		 } catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		 }
		 
		 String[] uniq= {"uniq",fichierSortie,fichierSortie+"Alpha.txt"};
		 try {
			Process proc=Runtime.getRuntime().exec(uniq);
			try {
				proc.waitFor();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			//System.out.println("fait");
		 } catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		 }
	}
	
	static String BinaireTochaine(StringBuilder a2){
	     
	    String res="";
	       for(int i=1;i<a2.length();i++){
	            if(i%8==0){
	                char a=(char)Integer.parseInt(a2.substring(i-8,i),2);
	                res=res+a;
	            }  
	        }
	      res=res+(char)Integer.parseInt(a2.substring(a2.length()-8,a2.length()),2);
	        return res;
	  }
	
	/*
	public static void main(String[] args) {
		ExtractionTermes T= new ExtractionTermes("listeTermes.txt","text.Unitex_snt/concord.txt","sorteTerme.txt");
		T.productionFichier();
	}*/

}
