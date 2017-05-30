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

//   construction du fichie XML  representant les phrases(sequences  Phrases )
public class XmlSequence {
	
	private  String fichierDestination;
	private  String fichieIn;
	
	public XmlSequence( String destinataion, String entree){
		
		fichierDestination= destinataion;
		fichieIn= entree;
		
	}
	
	public void conctuctionXml() throws FileNotFoundException, IOException{
		BufferedReader Lecteur = null;
		String ligne;
		ArrayList<String[]>  T= new ArrayList<String[]>();
		
		String Temp="";

				try {
					Lecteur=new BufferedReader(new  FileReader(fichieIn));
					BufferedWriter Sequence = new BufferedWriter(new FileWriter(fichierDestination));
					Sequence.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
					Sequence.newLine();
					Sequence.write("<Phrases>");
					Sequence.newLine();
					//Sequence.close();
					Pattern p = Pattern.compile("[{][S][}]");
					
					//Pattern p2=Pattern.compile("<(chapitre=.*?paragraphe=.*?)>");
					Pattern p2=Pattern.compile("<(type=.*?paragraphe=.*?)>");
					String attributP="";
					
					int i=0;
					while(((ligne = Lecteur.readLine()) != null)){
						Matcher mligne=p2.matcher(ligne);
						if(mligne.find()){
							attributP=mligne.group(1);
						}
						
						else{
						//System.out.println(ligne);
						String ligneTemp="";
						String[] temp=p.split(ligne);
						if (temp.length>1){
							ligneTemp=temp[0];
							String phrase=Temp+""+ligneTemp;
							Temp=temp[temp.length-1];
							//System.out.println(i);
							String texte=supprimeBalise(phrase);
							String test=new String("");
							if(texte.equals(test)==false){
								i++;
								Sequence.write("	<Phrase id='"+i+"' "+attributP+">");Sequence.newLine();
								//Sequence.write("		<idPhrase>"+i+("</idPhrase>")) ;Sequence.newLine();
								//Sequence.write("		<textePhrase>");
								//Sequence.newLine();
								Sequence.write("\t\t"+supprimeBalise(phrase));Sequence.newLine();
								//Sequence.write("		</textePhrase>");Sequence.newLine();
								Sequence.write("	</Phrase>");Sequence.newLine();
							}
						for(int j=0;j<temp.length-1;j++){
							
							//System.out.println(i);
							texte=supprimeBalise(temp[j]);
							test=new String("");
							if(texte.equals(test)==false){
								i++;
								Sequence.write("	<Phrase id='"+i+"' "+attributP+">");Sequence.newLine();
								//Sequence.write("		<idPhrase>"+i+("</idPhrase>")) ;Sequence.newLine();
								//Sequence.write("		<textePhrase>");
								//Sequence.newLine();
								Sequence.write("\t\t"+supprimeBalise(temp[j]));Sequence.newLine();
								//Sequence.write("		</textePhrase>");Sequence.newLine();
								Sequence.write("	</Phrase>");Sequence.newLine();
							}
							}
						}
						else{
							Temp=Temp+ligne;
						}
						
					}
					}
						String texte=supprimeBalise(Temp);
						String test=new String("");
						if(texte.equals(test)==false){
							i++;
							Sequence.write("	<Phrase id='"+i+"' "+attributP+">");Sequence.newLine();
							//Sequence.write("		<idPhrase>"+i+("</idPhrase>")) ;Sequence.newLine();
							//Sequence.write("		<textePhrase>");
							//Sequence.newLine();
							Sequence.write("\t\t"+supprimeBalise(Temp));Sequence.newLine();
							//Sequence.write("		</textePhrase>");Sequence.newLine();
							Sequence.write("	</Phrase>");Sequence.newLine();

						}
					Sequence.write("</Phrases>");
					Sequence.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					
			}
		
	}	
	
	public String  supprimeBalise(String ch){
		Pattern p2= Pattern.compile("([<.*].*?[>])([<.*].*?[>])*");
		Pattern p3= Pattern.compile("<.*?><.*?>+");
		
		Matcher a= p2.matcher(ch);
		
		if(a.find()){
			//System.out.println("trouvé dans >> "+ch);
		}
		else{
			//System.out.println("non trouvé dans >>"+ch);
		}
		
		String[] S=p2.split(ch);
		String res="";
		for(int i=0;i<S.length;i++){
			res=res+S[i];
		}
		
		//System.out.println("RES=>>"+res);
		return res;
		
		
	}
	/*
	public static void main(String[] args) throws FileNotFoundException, IOException{
		XmlSequence doc= new XmlSequence("Sequences.xml","Bokabo.txt");
		doc.conctuctionXml();
	}
*/
}
