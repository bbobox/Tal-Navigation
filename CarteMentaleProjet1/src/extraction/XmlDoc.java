package extraction;


import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import org.jdom2.Document;
import org.jdom2.Element;
import org.jdom2.input.SAXBuilder;
import org.jdom2.output.Format;
import org.jdom2.output.XMLOutputter;


/**
 * Classe de construction du Fichier XML Final 
 * @author etudiant
 *
 */
public class XmlDoc {
	
	private Element racine;
	private Element phrases;
	private Element termes;
	private Element EntiteN;
	private Element Relation;
	
	/**
	 * Construction du XML Correspondant à la Respresentation du Document
	 * @param xmlPhrase
	 * @param xmlTermes
	 * @param xmlEntiteN
	 * @param xmlRelation
	 * @param sortieXMl 
	 */
	public XmlDoc(String sortieXml,String xmlPhrase, String xmlTermes, String xmlEntiteN,String xmlRelation){
		
		SAXBuilder sxb = new SAXBuilder();
		
		racine = new Element("Doc");
		
		/* recuperation des phrase *:
		 */
		
		 try
	      {
	       
	        Document xmlphrases= sxb.build(new File(xmlPhrase));
	        phrases = xmlphrases.getRootElement().detach();
	        //Element e1=phrases.getChild("Sequence");
	        racine.addContent(phrases);
	        
	        Document xmltermes= sxb.build(new File(xmlTermes));
	        termes = xmltermes.getRootElement().detach();
	        racine.addContent(termes);
	        
	        
	        Document xmlEntites= sxb.build(new File(xmlEntiteN));
	        EntiteN= xmlEntites.getRootElement().detach();
	        racine.addContent(EntiteN);
	        
	        Document xmlrelations= sxb.build(new File(xmlRelation));
	        Relation= xmlrelations.getRootElement().detach();
	        racine.addContent(Relation);
	        
	        
	        
	        
	        
	      }
	      catch(Exception e){
	    	  System.err.println(e);
	      }
	    
		 Document doc= new Document(racine);
		 XMLOutputter sortie = new XMLOutputter(Format.getPrettyFormat());
		 try {
			sortie.output(doc, new FileOutputStream(sortieXml));
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		 
		  
		
	}

	
	/*
	public static void main(String[] args){
		XmlDoc d= new XmlDoc("XmlComplet.xml","Sequences.xml","XmlTermes.xml","XmlEntiteN.xml","XmlSemantiquesRelation.xml");
		
	} */
}
