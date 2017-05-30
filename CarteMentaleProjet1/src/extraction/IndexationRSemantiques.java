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
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Class Permettant d'indexer toute les relation semantique et de construire le Fichier XML Correspondant
 * @author etudiant
 *
 */
public class IndexationRSemantiques {
	private  String nomFichier;
	private Map<String,RelationSemantique> ensembleRelationsS;
	private Map<String, Termes> ensembleTermes;
	private String docCascade;
	
	/**
	 * Constructeur: qui prend en pamamètrele fichier contenant l'ensemble des relation semantique
	 */
	
	public IndexationRSemantiques(String fichierRelations, Map<String, Termes> termes,String doc){
		docCascade=doc;
		ensembleTermes=termes;
		
		ensembleRelationsS=new HashMap<String,RelationSemantique>();
		
		BufferedReader Lecteur = null;
		String ligne;
		String relation="";
		int i=0;
		try {
			Lecteur=new BufferedReader(new  FileReader(fichierRelations));
			Pattern p = Pattern.compile("<.*?>(.*?)<t1>(.*?)</t1>(.*?)<t2>(.*?)</t2></.*?>");
			
			while(((ligne = Lecteur.readLine()) != null)){
				//System.out.println(ligne);
				//i++;
				Matcher m=p.matcher(ligne);
				if (m.find()){
					relation=m.group(0);
				
					i++;
					RelationSemantique rS=new RelationSemantique(relation,ensembleTermes,docCascade,i);
					rS.indexationRelationS();
					//System.out.println(tokens+"ligne1--->>"+T.getList().get(1));
					//System.out.println("relation crée: >>>"+relation);
					ensembleRelationsS.put(relation,rS);
				}
				//i++;
				
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
	 *  Methode de contruction du fichier XMl Correpondant au Relation Semantiques 
	 * @param fichierSortie ( nom du Fichier de Sortie)
	 */
	
	public void xmlRSmenatiques( String fichierSortie){
		try {
			BufferedWriter xmlRelations = new BufferedWriter(new FileWriter(fichierSortie));
			 xmlRelations.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"); xmlRelations.newLine();
			 xmlRelations.write("<Relations>"); xmlRelations.newLine();
			 
			 

				Set cles = ensembleRelationsS.keySet(); 
				Iterator it = cles.iterator();
				Iterator i2;
				int i=0;
				
				while (it.hasNext()){
					i++;
					Object cle = it.next();
					Map valeur = ensembleRelationsS.get(cle).getList(); // On recupère la Map correspondant à la relations
					xmlRelations.write("	<Relation id=\""+ensembleRelationsS.get(cle).getIdRelation()+"\" type=\""+ensembleRelationsS.get(cle).getTypeRelation()+"\" idT1=\""+ensembleRelationsS.get(cle).getIdT1()+"\" idT2=\""+ensembleRelationsS.get(cle).getIdT2()+"\" >");xmlRelations.newLine();
					//xmlRelations.write("		<idRelation>"+ensembleRelationsS.get(cle).getIdRelation()+"</idRelation>");xmlRelations.newLine();
					//xmlRelations.write("		<typeRelation>"+ensembleRelationsS.get(cle).getTypeRelation()+"</typeRelation>");xmlRelations.newLine();
					xmlRelations.write("		<texteRelation>"+ensembleRelationsS.get(cle).getTexteRelation()+"</texteRelation>");xmlRelations.newLine();
					//xmlRelations.write(ensembleRelationsS.get(cle).getTexteRelation());//+"</texteRelation>");xmlRelations.newLine();
					//xmlRelations.write("		<idT1>"+ensembleRelationsS.get(cle).getIdT1()+"</idT1>");xmlRelations.newLine();
					//xmlRelations.write("		<idT2>"+ensembleRelationsS.get(cle).getIdT2()+"</idT2>");xmlRelations.newLine();
					
					xmlRelations.write("		<phrases>");xmlRelations.newLine();
					Set s=valeur.keySet();
					i2 = s.iterator();
					while (i2.hasNext()){
						   Object phrase = i2.next();
						   int nbOc= (Integer) valeur.get(phrase);
						  
						   if (nbOc!=0){
							   xmlRelations.write("			<phrase idPhrase=\""+phrase+"\" nbOcc=\""+nbOc+"\" />");//xmlRelations.newLine();
							  // xmlRelations.write("				<numPhrase>"+phrase+"</numPhrase>");xmlRelations.newLine();
							   //xmlRelations.write("				<nbOcc>"+nbOc+"</nbOcc>");xmlRelations.newLine();
							  // xmlRelations.write("			</phrase>");xmlRelations.newLine();
						   }
						  
					   }
					xmlRelations.write("		</phrases>");xmlRelations.newLine();
					xmlRelations.write("	</Relation>");xmlRelations.newLine();
				}
				xmlRelations.write("</Relations>");xmlRelations.newLine();	
				xmlRelations.close();
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	/*
	public static void main(String[] args){
		IndexationsTermes ITermes= new IndexationsTermes("sorteTerme.txtAlpha.txt","Bokabo.txt");
		ITermes.indexTermes();
		
		IndexationRSemantiques rS= new IndexationRSemantiques("/home/etudiant/workspace/CarteMentaleProjet/text.Unitex_csc/text.Unitex_0_0_snt/concord.txt",ITermes.getEnsembleTermes(),"test.UnitextRelation.txt");
		rS.xmlRSmenatiques("XmlSemantiquesRelation.xml");
	}*/

	
}
