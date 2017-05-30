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
 * Indexations des Mots du dictionnaire
 * @author Boka
 *
 */
public class IndexationTokens{
	private  Map<String,  Map>  listTokens ;  // indexation des Tokens ( Key: la chaine de caractère, et sont L'enemble (Map) des lignes dans lesquels ils apparaissent
	private  String nomFichier;
	
	/**
	 * Constructeur: qui prend en pamamètre la liste des Tokens
	 * @param listeTokens
	 */
	public IndexationTokens(String listeTokens){
		listTokens=new HashMap<String,  Map>();
		nomFichier=listeTokens;
	}

	/**
	 * Indexation des differents Tokens du Dictonnaire:
	 * @param: Fichier listant les tokens 
	 */
	
	public void  IndexTokens(){
		BufferedReader Lecteur = null;
		String ligne;
		String tokens;
		int i=0;
		try {
			Lecteur=new BufferedReader(new  FileReader(nomFichier));
			Pattern p = Pattern.compile("[\t]");
			
			while(((ligne = Lecteur.readLine()) != null)){
				//System.out.println(ligne);
				tokens= p.split(ligne)[0];
				Tokens T=new Tokens("text.Unitex_csc.tag.txt",2,tokens);
				T.IndexationTokens();
				//System.out.println(tokens+"ligne1--->>"+T.getList().get(1));
				listTokens.put(tokens,T.getList());
				System.out.println(tokens);
				i++;
			}	
			
			
		System.out.println("nbligne="+i);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			
		}
	
	}
	
	/**
	 * Fichiers Xml correspondant au Tokens
	 */
	public void XmlTokens(String fichierSortie){
		try {
			BufferedWriter xmlTokens = new BufferedWriter(new FileWriter(fichierSortie));
			xmlTokens.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");xmlTokens.newLine();
			xmlTokens.write("<tokens>");xmlTokens.newLine();
			int n=listTokens.size();
			
			Set cles = listTokens.keySet();
			Iterator it = cles.iterator();
			Iterator i2;
			int i=0;
			while (it.hasNext()){
				   i++;
				   Object cle = it.next(); // 
				   Map valeur = listTokens.get(cle); // On recupère la Map correspondant au mot
				   xmlTokens.write("	<token>");xmlTokens.newLine();
				   xmlTokens.write("		<idToken>"+i+"</idToken>");xmlTokens.newLine();
				   xmlTokens.write("		<forme>"+cle+"</forme>");xmlTokens.newLine();
				   Set s=valeur.keySet();
				   i2 = s.iterator();
				   xmlTokens.write("		<phrases>");xmlTokens.newLine();
				   while (i2.hasNext()){
					   Object phrase = i2.next();
					   int nbOc= (Integer) valeur.get(phrase);
					  
					   if (nbOc!=0){
						   xmlTokens.write("			<phrase>");xmlTokens.newLine();
						   xmlTokens.write("				<numPhrase>"+phrase+"</numPhrase>");xmlTokens.newLine();
						   xmlTokens.write("				<nbOcc>"+nbOc+"</nbOcc>");xmlTokens.newLine();
						   xmlTokens.write("			</phrase>");xmlTokens.newLine();
					   }
					  
				   }
				   xmlTokens.write("		</phrases>");xmlTokens.newLine();
				   xmlTokens.write("	</token>");;xmlTokens.newLine();
				}
			xmlTokens.write("	</tokens>");xmlTokens.newLine();
			
			xmlTokens.close();
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
	}
	
	/*
	public static void main(String[] args) throws FileNotFoundException, IOException{
		System.setProperty( "file.encoding", "UTF-8" );
		IndexationTokens ITokens= new IndexationTokens("tokensordre");
		ITokens.IndexTokens();
		ITokens.XmlTokens("xmlTokens.xml");
		//System.out.print("nombre de Un dans la ligne---->"+ITokens.listTokens.get("une"));
		System.out.print("nombre de Un dans la ligne---->"+ITokens.listTokens.get("un").size());
	}*/
}
