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
 * @author BOKA Yao
 *
 */
public class IndexationsEntiteN{
	private  String nomFichier; // nom du fichier 
	private Map<String,EntiteN> ensembleEntiteN;
	private String textephrase;
	private String listTermes;
	private String fichierSortieTagEN;
	private Map<String, Termes> ensemblesTermes;
	
	/**
	 * Constructeur du document 
	 * @ fichier dans lequel est repertorié toutes les entintée nomée reconnu dans le texte
	 */
	public IndexationsEntiteN(String listeEntite,String texte, String termes,String fileTagEN, Map<String, Termes> eTermes){
		nomFichier=listeEntite;
		ensembleEntiteN= new HashMap<String,EntiteN>();
		textephrase=texte;
		listTermes=termes;
		fichierSortieTagEN= fileTagEN; 
		ensemblesTermes=eTermes;
	}
	/**
	 * Indexation des Entites
	 */
	public void IndexationsEntiteN(){
		BufferedReader Lecteur = null;
		String ligne;
		String entiteN;
		int i=0;
		try {
			Lecteur=new BufferedReader(new  FileReader(nomFichier));
			Pattern p = Pattern.compile("[\t]");
			
			while(((ligne = Lecteur.readLine()) != null)){
				i++;
				entiteN= ligne;
				EntiteN e=new EntiteN(fichierSortieTagEN,entiteN,listTermes,textephrase,ensemblesTermes);
				e.indexationEntiteN();
				//System.out.println(tokens+"ligne1--->>"+T.getList().get(1));
				//listTermes.put(terme,T.getList());
				 ensembleEntiteN.put(entiteN,e);
					//System.out.println(terme);
				//i++;
				
			}	
			
		System.out.println("nbligne="+i);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			
		}
	}
	

	/**
	 * Fichiers Xml correspondant au Entite Nommées
	 * @param fichierSortie : Le fichier XMl renvoyé en sortie
	 */
	public void XmlENtiteN(String fichierSortie){
		try {
			BufferedWriter xmlEntiteN = new BufferedWriter(new FileWriter(fichierSortie));
			xmlEntiteN.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");xmlEntiteN.newLine();
			xmlEntiteN.write("<EntitesN>");xmlEntiteN.newLine();
			
			Set cles = ensembleEntiteN.keySet(); 
			Iterator it = cles.iterator();
			Iterator i2;
			int i=0;
			
			while (it.hasNext()){
				i++;
				Object cle = it.next();
				Map valeur = ensembleEntiteN.get(cle).getList();// On recupère la Map correspondant au mot
				
				if ((ensembleEntiteN.get(cle).getIdTerme())!=0){
					xmlEntiteN.write("	<EntiteN id='"+i+"'  type='"+ensembleEntiteN.get(cle).getType()+"'  termCorrespondant='"+ensembleEntiteN.get(cle).getIdTerme()+"'>");xmlEntiteN.newLine();
					//xmlEntiteN.write("		<idTerme>"+ensembleEntiteN.get(cle).getIdTerme()+"</idTerme>");xmlEntiteN.newLine();
					}
				else{
					xmlEntiteN.write("	<EntiteN id='"+i+"' type='"+ensembleEntiteN.get(cle).getType()+"' >");xmlEntiteN.newLine();
				}
				//xmlEntiteN.write("		<idEntiteN>"+i+"</idEntiteN>");xmlEntiteN.newLine();
				xmlEntiteN.write("		<nomEntiteN>"+ensembleEntiteN.get(cle).getLibelle()+"</nomEntiteN>");xmlEntiteN.newLine();
				
				//xmlEntiteN.write(cle);
				xmlEntiteN.write("		<phrases>");xmlEntiteN.newLine();
				Set s=valeur.keySet();
				i2 = s.iterator();
				while (i2.hasNext()){
					   Object phrase = i2.next();
					   int nbOc= (Integer) valeur.get(phrase);
					  
					   if (nbOc!=0){
						   xmlEntiteN.write("			<phrase idPhrase='"+phrase+"' nbOcc='"+nbOc+"' />");xmlEntiteN.newLine();
						  // xmlEntiteN.write("				<numPhrase>"+phrase+"</numPhrase>");xmlEntiteN.newLine();
						  // xmlEntiteN.write("				<nbOcc>"+nbOc+"</nbOcc>");xmlEntiteN.newLine();
						  // xmlEntiteN.write("			</phrase>");xmlEntiteN.newLine();
					   }
					  
				   }
				xmlEntiteN.write("		</phrases>");xmlEntiteN.newLine();
				xmlEntiteN.write("	</EntiteN>");xmlEntiteN.newLine();
			}
			xmlEntiteN.write("</EntitesN>");xmlEntiteN.newLine();	
			xmlEntiteN.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	
	/*public static void main(String[] args) throws FileNotFoundException, IOException{
		System.setProperty( "file.encoding", "UTF-8" );
		IndexationsEntiteN IEntiteN= new IndexationsEntiteN("listEntiteUniq.txt","Bokabo.txt","sorteTerme.txt","text.Unitex.tag.snt");
		IEntiteN.IndexationsEntiteN();
		IEntiteN.XmlENtiteN("XmlEntiteN.xml");
		//System.out.print("nombre de etiquetage morpho Syntaxique dans la ligne---->"+ITermes.listTermes.get("etiquetage morpho Syntaxique").get(6));
		//System.out.print("nombre de Un dans la ligne---->"+ITokens.listTokens.get("un").size());
	}*/
	


}
