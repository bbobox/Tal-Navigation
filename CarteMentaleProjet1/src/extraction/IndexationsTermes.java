package extraction;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.regex.Pattern;

/**
 * Indexations des Termes
 * @author BOKA Yao
 *
 */
public class IndexationsTermes {
	private  String nomFichier;    // liste des Termes
	private Map<String,Termes> ensembleTermes;   // ensemble des termes
	
	private  String nomDocument;
	
	
	/**
	 * Constructeur: qui prend en pamamètre la liste des Termes
	 * @param listeTermes
	 */
	public IndexationsTermes( String listeTermes,String doc){
		 nomFichier=listeTermes;
		 nomDocument=doc;
		 //listTermes=new HashMap<String,  Map>();
		 setEnsembleTermes(new HashMap<String,  Termes>());
	}
	
	
	public void indexTermes(){
		BufferedReader Lecteur = null;
		String ligne;
		String terme;
		int i=0;
		try {
			Lecteur=new BufferedReader(new  FileReader(nomFichier));
			Pattern p = Pattern.compile("[\t]");
			
			while(((ligne = Lecteur.readLine()) != null)){
				//System.out.println(ligne);
					if(!ligne.startsWith("#")){
						i++;
					terme= p.split(ligne)[0];
					Termes T=new Termes(nomDocument,terme,i);
					T.indexationTerme();
					//System.out.println(tokens+"ligne1--->>"+T.getList().get(1));
					//listTermes.put(terme,T.getList());
					getEnsembleTermes().put(terme,T);
					//System.out.println(terme);
				//i++;
				}
			}	
			
			
		//System.out.println("nbligne="+i);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			
		}
	
	}
	
	
	/**
	 * Fichiers Xml correspondant au Termes
	 * @param fichierSortie : Le fichier XMl renvoyé en sortie
	 */
	
	public void xmlTermes(String fichierSortie){
		try {
			BufferedWriter xmlTermes = new BufferedWriter(new FileWriter(fichierSortie));
			xmlTermes.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");xmlTermes.newLine();
			xmlTermes.write("<termes>");xmlTermes.newLine();
			
			Set cles = getEnsembleTermes().keySet(); 
			Iterator it = cles.iterator();
			Iterator i2;
			int i=0;
			
			while (it.hasNext()){
				i++;
				Object cle = it.next();
				Map valeur = getEnsembleTermes().get(cle).getList(); // On recupère la Map correspondant au mot
				xmlTermes.write("	<terme id=\""+getEnsembleTermes().get(cle).getIdTerme()+"\">");xmlTermes.newLine();
				//xmlTermes.write("		<idTerme>"+getEnsembleTermes().get(cle).getIdTerme()+"</idTerme>");xmlTermes.newLine();
				xmlTermes.write("		<nomTerme>"+cle+"</nomTerme>");xmlTermes.newLine();
				xmlTermes.write("		<phrases>");xmlTermes.newLine();
				Set s=valeur.keySet();
				i2 = s.iterator();
				while (i2.hasNext()){
					   Object phrase = i2.next();
					   int nbOc= (Integer) valeur.get(phrase);
					  
					   if (nbOc!=0){
						   xmlTermes.write("			<phrase idPhrase=\""+phrase+"\" nbOcc=\""+nbOc+"\" />");xmlTermes.newLine();
						   //xmlTermes.write("				<numPhrase>"+phrase+"</numPhrase>");xmlTermes.newLine();
						   //xmlTermes.write("				<nbOcc>"+nbOc+"</nbOcc>");xmlTermes.newLine();
						   //xmlTermes.write("			</phrase>");xmlTermes.newLine();
					   }
					  
				   }
				xmlTermes.write("		</phrases>");xmlTermes.newLine();
				xmlTermes.write("	</terme>");xmlTermes.newLine();
			}
			xmlTermes.write("</termes>");xmlTermes.newLine();	
			xmlTermes.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		
	}
	/*
	
	public static void main(String[] args) throws FileNotFoundException, IOException{
		System.setProperty( "file.encoding", "UTF-8" );
		IndexationsTermes ITermes= new IndexationsTermes("sorteTerme.txtAlpha.txt","Bokabo.txt");
		ITermes.indexTermes();
		ITermes.xmlTermes("XmlTermes.xml");
		//System.out.print("nombre de etiquetage morpho Syntaxique dans la ligne---->"+ITermes.listTermes.get("etiquetage morpho Syntaxique").get(6));
		//System.out.print("nombre de Un dans la ligne---->"+ITokens.listTokens.get("un").size());
	}  */


	public Map<String,Termes> getEnsembleTermes() {
		return ensembleTermes;
	}


	public void setEnsembleTermes(Map<String,Termes> ensembleTermes) {
		this.ensembleTermes = ensembleTermes;
	}

}
