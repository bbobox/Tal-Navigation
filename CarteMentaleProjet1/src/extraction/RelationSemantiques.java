package extraction;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Contsructution du d'extractions des relations semantiques
 * @author etudiant
 *
 */
public class RelationSemantiques {
	private  String fichierDestination;
	private  String fichieIn;
	
	/** Constructeur */
	public RelationSemantiques(String destinataion, String entree){
		fichierDestination= destinataion;
		fichieIn= entree;
		
	}
	
	public void extrationRelations() throws FileNotFoundException, IOException{
		BufferedReader Lecteur = null;
		String ligne;
		ArrayList<String[]>  T= new ArrayList<String[]>();
		
		String Temp="";

		try {
			Lecteur=new BufferedReader(new  FileReader(fichieIn));
			//BufferedWriter Sequence = new BufferedWriter(new FileWriter(fichierDestination));
			BufferedWriter ListEntite= new BufferedWriter(new FileWriter(fichierDestination));
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
							ListEntite.write(text.get(k));ListEntite.newLine();
						}
				}
					//System.out.println(supprimeBalise(phrase));
				for(int j=0;j<temp.length-1;j++){
					
					//System.out.println(i);
					//texte=supprimeBalise(temp[j]);
					text= attributsEntite(temp[j]);
					test=new String("");
					if(temp[j].equals(test)==false){
							i++;
							
							for(int k=0;k<text.size();k++){
							//Sequence.write("	<entiteN>"+text.get(k)+("</entiteN>")) ;Sequence.newLine();
							
								cpt++;
								ListEntite.write(text.get(k));ListEntite.newLine();

							}
					}
					//System.out.println(supprimeBalise(phrase));
					//System.out.print(supprimeBalise(temp[j]));
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
						cpt++;
						
						ListEntite.write(text.get(k));ListEntite.newLine();
						
					}
			}
			ListEntite.close();

		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			
		}
		
	}	
		
		public ArrayList<String> attributsEntite(String Text){
			String type;
			String type2;
			String entiteN;
			String line = "<bla><t1>ha</t2></bla><bla>machin</bla><destrucs>encore pleins de machins</destrucs>";
			Pattern p = Pattern.compile("<.*?>(.*?)<t1>(.*?)</t1>(.*?)<t2>(.*?)</t2></.*?>");
			Matcher m = p.matcher(Text);
			// 0 -> première occurrence,  1 -> 2e occurrence, 2 -> 3e .....
			ArrayList<String> res=new ArrayList<String>();
			System.out.println(">>>>>"+Text);
			while(m.find()){
			    System.out.println(m.group(0)); // On affiche la 2e occurrence
			    res.add(m.group(0));
			   // System.out.println(m.group(1));
			// Affiche : machin
			
			}

			return res;
		}
		
		/*
		public static void main(String[] args) throws FileNotFoundException, IOException{
			
			// String line = "Une relation de definition est la suivante: <definir>définir la <t1>To</t1> comme la <t2>Ti</t2></definir>.";
			//Pattern p = Pattern.compile("<.*?>(.*?)<.*?>(.*?)</.*?>(.*?)<.*?>(.*?)</.*?></.*?>");
			//Matcher m = p.matcher(line);
			// 0 -> première occurrence,  1 -> 2e occurrence, 2 -> 3e .....
			//while(m.find()){
			//    System.out.println(m.group(0)); // On affiche la 2e occurrence
			//    System.out.println(m.group(1));
			// Affiche : machin
			
			}
			
			RelationSemantiques doc= new RelationSemantiques("Semantiques.txt","text.Unitex_csc.tag.txt");
			doc.extrationRelations();
			
		}
		*/
}
