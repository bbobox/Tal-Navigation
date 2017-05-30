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
 * Une Relation Semantique Represente
 * @author etudiant
 *
 */
public class RelationSemantique {
	private String typeRelation;
	private String t1;
	private String t2;
	private int idT1;
	private int idT2;
	private String texteRelation;
	private Map<String,Termes> listeTermes;
	private Map<Integer,Integer> list; // liste des phrase dans lesquelles elles apparait et son nombre d'occurence
	private String nomDoc;
	private String relationBalise;
	private int idRelation;
	
	
	public RelationSemantique(String relationB,Map<String,Termes> listeTerme, String doc, int id){
		setList(new HashMap<Integer,Integer>());
		listeTermes=listeTerme;
		nomDoc=doc;
		
		//identifiant e la relation
		setIdRelation(id);
		
		
		// texte de la relation
		relationBalise=relationB;
		setTexteRelation(supprimeBalise(relationB));
		System.out.println(getTexteRelation());
		
		// recuperation de T1
		Pattern p = Pattern.compile("<t1> (.*?)</t1>");
		Matcher mT1=p.matcher(relationBalise);
		if (mT1.find()){
			t1=mT1.group(1);
			
			System.out.println(" T1 est >>>>"+t1);
			
			//if (listeTermes.get(t1)!=null){
			setIdT1(listeTermes.get(t1).getIdTerme());
			System.out.println("idterme1="+getIdT1());
			//}
		}
		
		// recuperation de T1
		p= Pattern.compile("<t2> (.*?)</t2>");
		Matcher mT2=p.matcher(relationBalise);
		if (mT2.find()){
			t2=mT2.group(1);
			System.out.println("t2>>"+mT2.group(1));
			setIdT2(listeTermes.get(t2).getIdTerme());
			System.out.println("idterme2="+getIdT2());		
		}
		
		// reciperation du type de relation*
		p= Pattern.compile("^<(.*?)>");
		Matcher mType=p.matcher(relationBalise);
		if (mType.find()){
			//t2=mType.group(1);
			setTypeRelation(mType.group(1));
			System.out.println("type>>"+mType.group(1));
		}
		
		// Ligne dans lequel il apparait et son nombre d'occurence
		
		
	}
	
	
	/**
	 * Indexation de la relation Semantique: renvoi la liste des Phrases dans lesquelles la relation est identifiée  et son nombre d'occurences dans chacunes d'elles 
	 */
	
	public void indexationRelationS(){
		BufferedReader Lecteur = null;
		String ligne;
		String terme;
		String Temp="";
		int nbligne;
		try {
			Lecteur=new BufferedReader(new  FileReader(nomDoc));
			Pattern p = Pattern.compile("[{][S][}]");
			
			int i=0;
			int cpt=0;
			while(((ligne = Lecteur.readLine()) != null)){
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
						nbligne= stringOccur(phrase, relationBalise);
						getList().put(i, nbligne);
						
					}
					
				for(int j=1;j<temp.length-1;j++){
					
					test=new String("");
					if(temp[j].equals(test)==false){
						i++;
						nbligne= stringOccur(temp[j], relationBalise);
						getList().put(i, nbligne);
						
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
				nbligne= stringOccur(Temp, relationBalise);
				//System.out.println(i+">>:"+nbligne+"->>Temp"+Temp);
				getList().put(i, nbligne);
				//System.out.println("ligne="+Temp);	
				
			}
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
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
	 
	
	
	
	
	
	/**
	 * Suprime les Balise de la relation semantiques 
	 * @param ch
	 * @return
	 */
	public String  supprimeBalise(String ch){
		Pattern p2= Pattern.compile("[<].*?[>]");
		
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
	public static void main(String[] args){
		IndexationsTermes ITermes= new IndexationsTermes("/home/etudiant/workspace/CarteMentaleProjet/sorteTerme.txtAlpha.txt","Bokabo.txt");
		ITermes.indexTermes();
		
		
		RelationSemantique R= new RelationSemantique("<definir>définir la<t1> To</t1> comme la<t2> Ti</t2></definir>",ITermes.getEnsembleTermes(),"test.UnitextRelation.txt",0);
		R.indexationRelationS();
		System.out.print(R.getList().get(4));
	}
	*/


	public Map<Integer,Integer> getList() {
		return list;
	}


	public void setList(Map<Integer,Integer> list) {
		this.list = list;
	}


	public int getIdRelation() {
		return idRelation;
	}


	public void setIdRelation(int idRelation) {
		this.idRelation = idRelation;
	}


	public String getTypeRelation() {
		return typeRelation;
	}


	public void setTypeRelation(String typeRelation) {
		this.typeRelation = typeRelation;
	}


	public String getTexteRelation() {
		return texteRelation;
	}


	public void setTexteRelation(String texteRelation) {
		this.texteRelation = texteRelation;
	}


	public int getIdT1() {
		return idT1;
	}


	public void setIdT1(int idT1) {
		this.idT1 = idT1;
	}


	public int getIdT2() {
		return idT2;
	}


	public void setIdT2(int idT2) {
		this.idT2 = idT2;
	}
	
}
