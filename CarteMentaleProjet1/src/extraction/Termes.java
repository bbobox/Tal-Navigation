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
 * Termes  Representé:
 * 		-Un identifiant
 * 		-Son nom d'occurrence dans chaque Phrase
 */
public class Termes {
	private  Map<Integer, Integer> list ; // Ensemble des Lignes dans lequelles appartient le termes
	private String nomTerme;					// nom du terme
	private String nomDoc;					// nom du fichier representant le document
	private int idTerme;
	
	
	 /**Constructeur 
	  * @param : doc qui le nom de document à traiter
	  * @param : terme : le nom du terme
	  * @param : id: indentifiant unique du terme
	  * */
	public Termes(String doc,String terme,int id){
		list= new HashMap <Integer, Integer>();
		nomDoc=doc;
		nomTerme=terme;
		setIdTerme(id);
		
	}
	
	public Map<Integer, Integer> getList() {
		return list;
	}

	public void setList(Map<Integer, Integer> list) {
		this.list = list;
	}  
	
	/**
	 * Indexation du Terme: renvoi la liste des Phrases dans lesquelles le termes est identifié  et son nombre d'occurences dans chacunes d'elles 
	 */
	
	public void indexationTerme()
	{
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
					//System.out.println(i);
					//nbligne= stringOccur(phrase, nomToken); //1
					//System.out.println(i+">>:"+nbligne+"->>Temp"+Temp);
					//list.put(i, nbligne);
					//System.out.println("ligne="+phrase);
					
					String test=new String("");
					if(phrase.equals(test)==false){
						i++;
						nbligne= stringOccur(phrase, nomTerme);
						//System.out.println(i+">>:"+nbligne+"->>Temp"+phrase);
						list.put(i, nbligne);
						
					}
					
				for(int j=1;j<temp.length-1;j++){
					
					//System.out.println(i);
					//texte=supprimeBalise(temp[j]);
					//i++;
					//nbligne= stringOccur(temp[j], nomToken);
					//System.out.println(i+">>:"+nbligne+"->>Temp"+temp[j]);
					test=new String("");
					if(temp[j].equals(test)==false){
						i++;
						nbligne= stringOccur(temp[j], nomTerme);
						//System.out.println(i+">>:"+nbligne+"->>Temp"+Temp);
						list.put(i, nbligne);
						//System.out.println("ligne="+temp[j]);
						
					}
					}
				}
				else{
					Temp=Temp+ligne;
				}
				
			}	
			
				//String texte=supprimeBalise(Temp);
			//i++;
			String test=new String("");
			if(Temp.equals(test)==false){
				i++;
				nbligne= stringOccur(Temp, nomTerme);
				//System.out.println(i+">>:"+nbligne+"->>Temp"+Temp);
				list.put(i, nbligne);
				//System.out.println("ligne="+Temp);	
				
			}
			
		
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			
		}
	
		
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
		 Matcher matcher = Pattern.compile(" "+regex+" ").matcher(text);
		    Matcher matcher2 = Pattern.compile("^"+regex+" ").matcher(text);
		    Matcher matcher3 = Pattern.compile(" "+regex+".").matcher(text);
		    Matcher matcher4 = Pattern.compile("'"+regex+" ").matcher(text);
		    int occur = 0;
		    while(matcher.find()|matcher2.find()|matcher3.find()|matcher4.find()) {
		        occur ++;
		        //System.out.print(">>>>"+text);
		        //if(matcher.find()){
		       // 	 System.out.print(">>>>"+matcher.group(1));
		        //}
		        //System.out.print(">>>>"+matcher.group(1));
		    }
		    return occur;
	}
	 
	 /*
	 public static void main(String[] args) throws FileNotFoundException, IOException{
		 Termes T= new Termes("Bokabo.txt","Ti",4);
		 T.indexationTerme();
		System.out.print(T.list.get(4));
		}
	*/
	public int getIdTerme() {
		return idTerme;
	}

	public void setIdTerme(int idTerme) {
		this.idTerme = idTerme;
	} 

}
