package extraction;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;

import org.jdom2.Element;
import org.jdom2.input.SAXBuilder;

public class TestJDom {
	
	 private org.jdom2.Document document;
	 private Element racine;
	 private String fileTerms;
	 private String fichieXmlYatea;
	 
	 public TestJDom(String fileEntre,String fileSortie){
		 fileTerms=fileSortie;
		 fichieXmlYatea=fileEntre;
		 
		  //On crée une instance de SAXBuilder
	      SAXBuilder sxb = new SAXBuilder();
	      try
	      {
	         //On crée un nouveau document JDOM avec en argument le fichier XML
	         //Le parsing est terminé ;)
	         document = sxb.build(new File(fichieXmlYatea));
	         racine = document.getRootElement();
	      }
	      catch(Exception e){
	    	  System.err.println(e);
	      }
	     // racine = document.getRootElement();
	 }
	 
	public void listeTermes()
	 {
	    //On crée une List contenant tous les noeuds "etudiant" de l'Element racine
	    Element lisTermesCandidats = racine.getChild("LIST_TERM_CANDIDATES");
	    List termes=lisTermesCandidats.getChildren("TERM_CANDIDATE");
	    
	    BufferedWriter fTermes;
	    
		// File fterms = new File(fileTerms);
		 //if (fterms.exists()){
		//	 fterms.deleteOnExit();
		 //}
	    
	    try {
			fTermes = new BufferedWriter(new FileWriter(fileTerms));
			
			 //On crée un Iterator sur notre liste
		    Iterator i = termes.iterator();
		    while(i.hasNext())
		    {
		       //On recrée l'Element courant à chaque tour de boucle afin de
		       //pouvoir utiliser les méthodes propres aux Element comme :
		       //sélectionner un nœud fils, modifier du texte, etc...
		       Element courant = (Element)i.next();
		       //On affiche le nom de l’élément courant
		       //System.out.println(courant.getChild("FORM").getText());
		       fTermes.write(courant.getChild("FORM").getText());fTermes.newLine();
		    }
		    fTermes.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    

	   
	 }
	 
	 
	 /*
	 public static void main(String[] args)
	   {
		 String[] cmdcpy= { "cp","yatea.dtd","Boka/default/xml"};
		 try {
			Process proc=Runtime.getRuntime().exec(cmdcpy);			
		 } catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();}
		 
		 TestJDom T= new TestJDom("Boka/default/xml/candidates.xml","listeTermes.txt"); 
		 T.listeTermes();
	   }
	*/
}
