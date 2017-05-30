package extraction;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


/**
 * EntiteNomme  Representé:
 * 		-Un identifiant
 * 		-Son nom d'occurrence dans chaque Phrase du Texte
 */
public class EntiteN {
	private  Map<Integer, Integer> list ; // Ensemble des Lignes dans lequelles appartient l'entite
	private String terme;
	private int idTerme;
	private String type;
	private  String nomDoc;
	private String entiteN;
	private String fileListTerms;
	private String textephrase;
	private String libelle;
	private Map<String, Termes> termes;
	//private int iD;
	
	/** Construeteur de la Class et prend en paramètre
	 * -nom du document
	 * -nom de l'entite
	 * -texte decoupé en phrases
	 */
	public EntiteN(String doc,String entite, String fichiersTerms,String texte, Map<String, Termes> ensembleTermes ){
		list= new HashMap <Integer, Integer>();
		nomDoc=doc;
		entiteN=entite;
		terme="";
		fileListTerms=fichiersTerms;
		textephrase=texte;
		setIdTerme(0);
		
		termes=  ensembleTermes;
		
		Pattern p=Pattern.compile("<.*?><(.*?)>");
		Matcher m=p.matcher(entite);
		
		if (m.find()){
			System.out.println(">>>"+m.group(1));
			setType(m.group(1));
		}
		
		
		Pattern p2 = Pattern.compile("<.*?>(<.*?>)+(.*?)</.*?>(</.*?>)+");
		//Pattern p3 = Pattern.compile("<.*?><.*?><(.*?)</.*?></.*?>")
		Matcher m2=p2.matcher(entiteN);
		//String entiteSansBalise="";
		if(m2.find()){
			setLibelle(m2.group(2));
			System.out.print(">>>"+m2.group(2));
		}
	}
	
	public Map<Integer, Integer> getList() {
		return list;
	}

	public void setList(Map<Integer, Integer> list) {
		this.list = list;
	} 
	
	/**
	 * Indexationd de l'entite: renvoi la liste des Phrase dans lesquelles le document est identifié  et son nombre d'occurence dans chacunes d'elles 
	 */
	
	public void indexationEntiteN(){
		BufferedReader Lecteur = null;
		String ligne;
		String Temp="";
		int nbligne;
		/* Indexation par rapports aux lignes */
		try {
			Lecteur=new BufferedReader(new  FileReader(nomDoc));
			Pattern p = Pattern.compile("[{][S][}]");
			
			int i=0;
			int cpt=0;
			while(((ligne = Lecteur.readLine()) != null)){
				//System.out.println(ligne);
				String ligneTemp="";
				String[] temp=p.split(ligne);
				if (temp.length>1){
					//i++;
					ligneTemp=temp[0];
					String phrase=Temp+ligneTemp;
					Temp=temp[temp.length-1];

					
					String test=new String("");
					if(phrase.equals(test)==false){
						i++;
						nbligne= stringOccur(phrase, entiteN);
						list.put(i, nbligne);
						
					}
					
				for(int j=1;j<temp.length-1;j++){

					test=new String("");
					if(temp[j].equals(test)==false){
						i++;
						nbligne= stringOccur(temp[j], entiteN);
						list.put(i, nbligne);
						
					}
					}
				}
				else{
					Temp=Temp+ligne;
				}
				
			}	
			
			String test=new String("");
			if(Temp.equals(test)==false){
				i++;
				nbligne= stringOccur(Temp, entiteN);
				list.put(i, nbligne);
				
			
				
			}
			
		
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			
		}
	
		
		
		/* Indexation par rapports aux Termes */
		//IndexationsTermes ITermes= new IndexationsTermes(fileListTerms,textephrase);
		//ITermes.indexTermes();
		//BufferedReader LecteurTermes = null;
		String lignetermes;
		
		if (termes.get(libelle)!=null){
			idTerme=termes.get(libelle).getIdTerme();
		}
		/*try{
			//LecteurTermes=new BufferedReader(new  FileReader(fileListTerms));
			//Pattern p = Pattern.compile("<.*?><.*?>(.*?)</.*?></.*?>");
			//Matcher m=p.matcher(entiteN);
			//String entiteSansBalise="";
			//if(m.find()){
			//	entiteSansBalise=m.group(1);;
			//}
			//boolean test=false;
			//while(((lignetermes = LecteurTermes.readLine()) != null) && test==false){
			//	String t=lignetermes.split("[\t]")[0];
			//	if (t.equals(libelle)==true){
			//		test=true;
			//		terme=t;
			//		idTerme=ITermes.getEnsembleTermes().get(terme).getIdTerme();
			//	}
			//}
			
		}catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			
		} */
		
	}
	
	
	/**
	 *  Renvoi le nombre d'occurence 
	 * @return
	 */
	 public static final int stringOccur(String text, String string) {
		    return regexOccur(text, Pattern.quote(string));
		}
	/**
	 * Renvoie le nombre d'occurrences du pattern spécifié dans la chaine de caractères spécifiée
	 * @param text chaine de caractères initiale
	 * @param regex expression régulière dont le nombre d'occurrences doit etre compté
	 * @return le nombre d'occurrences du pattern spécifié dans la chaine de caractères spécifiée
	 */
	 
	 public static final int regexOccur(String text, String regex) {
	    Matcher matcher = Pattern.compile(regex).matcher(text);
	    int occur = 0;
	    while(matcher.find()) {
	        occur ++;
	    }
	    return occur;
	}
	
	 
	 
	 /*public static void main(String[] args) {
		// IndexationsTermes ITermes= new IndexationsTermes("abCompletTermes.txtAlpha.txt","abConv.txt");
		 ITermes.indexTermes();
		 
		 EntiteN EN= new EntiteN("abConv.tag.txt","<enamex><location>Venise</location></enamex>","abCompletTermes.txtAlpha.txt","abConv.txt", ITermes.getEnsembleTermes());
		EN.indexationEntiteN();
		System.out.print(EN.getList().get(5));
		 System.out.println();
		 
		 //
	
		 
		 Pattern p= Pattern.compile("[.](([\t]+)|([ ]+))");
		 Matcher m= p.matcher("nnd.	sdsf");
		 String[] s=p.split("{S}3. Quand traduit-on ?");
		 if (m.find()){
			// System.out.println(m.group(0));
			 //System.out.println("\""+s.length);
			 for(int i=0;i<s.length-1;i++){
				 System.out.print(s[i]+".");
			 }
			 System.out.print(s[s.length-1]);
		 }
		 //System.out.println(s[]);
		} */

	public int getIdTerme() {
		return idTerme;
	}

	public void setIdTerme(int idTerme) {
		this.idTerme = idTerme;
	}

	public String getType() {
		return type;
	}

	public void setType(String type) {
		this.type = type;
	}

	public String getLibelle() {
		return libelle;
	}

	public void setLibelle(String libelle) {
		this.libelle = libelle;
	} 
	 
	
}
