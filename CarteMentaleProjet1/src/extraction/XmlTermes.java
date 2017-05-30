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
 * Definition de a classe d'extraction des Termes
 */

public class XmlTermes {
	
	private  String fichierDestination;
	private  String fichieIn;
	
	/** Constructeur  */
	public XmlTermes(){
		
	}
	
	/**
	 * Methode de Constructiond e fichier XMl:
	 * -Prend entrée le nom des fichier d'entrée et sortie et  ens oertie un fichier Xml correspondant aux termes 
	 */
	
	public void conctuctionXml() throws FileNotFoundException, IOException{
		BufferedReader Lecteur = null;
		String ligne;
		ArrayList<String[]>  T= new ArrayList<String[]>();
		
		String Temp="";

				try {
					Lecteur=new BufferedReader(new  FileReader(fichieIn));
					BufferedWriter Sequence = new BufferedWriter(new FileWriter(fichierDestination));
					Sequence.write("<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>");
					Sequence.newLine();
					Sequence.write("<Termes>");
					Sequence.newLine();
					//Sequence.close();
					Pattern p = Pattern.compile("[{][S][}]");
					
					//Pattern p2= Pattern.compile("[<](.)*[>]");
					
					int i=0;
					while(((ligne = Lecteur.readLine()) != null)){
						
						//System.out.println(ligne);
						String ligneTemp="";
						String[] temp=p.split(ligne);
						if (temp.length>1){
							ligneTemp=temp[0];
							String phrase=Temp+ligneTemp;
							Temp=temp[temp.length-1];
							//System.out.println(i);
						for(int j=0;j<temp.length-1;j++){
							
							}
						}
						else{
							Temp=Temp+ligne;
						}
						
						
					}
					//	String texte=supprimeBalise(Temp);
						
					Sequence.write("</Termes>");
					Sequence.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					
			}
				
		/**
		 * Verifier Si un mot appartient à une texte.
		 */
				
		
	}
	
	/*
	
	public void estDansTexte( String mot, String texte){
		Pattern p2= Pattern.compile(mot);
		Matcher m= p2.matcher(texte);
		
		
	} */

}
