package extraction;


import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Iterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.jdom2.Document;
import org.jdom2.Element;
import org.jdom2.JDOMException;
import org.jdom2.input.SAXBuilder;
/**
 *  Class Qui Consiste à Manuipuler un CORPUS SEGMENTE
 *
 */

public class ConvertionSengmentToText {
	
	private Document doc;
	private String fichierSortie;
	private Element racine;
	
	public ConvertionSengmentToText(String document, String sortie){
		fichierSortie=sortie;
		SAXBuilder sxb = new SAXBuilder();
		try {
			doc= sxb.build(new File(document));
			racine = doc.getRootElement();
			
		} catch (JDOMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void ConvertionText(){
		java.util.List<Element>  chapitres =   racine.getChildren();
				//getgetChildren();   //getChildren();
				//getChildren("paragraphe"); // recuperation de tous les chapitre
		
		 try {
			BufferedWriter Lecteur=new BufferedWriter(new  FileWriter(fichierSortie));
			
			Iterator i = chapitres.iterator();
			int nb=0;
			String idChap="";
			String idSection="";
			String idSousSection="";
			String idSegment="";
			String idParagraphe="";
			/* recuperation des phrases Contenu dans les paragraphes */
			
			while(i.hasNext())
			    {
				 
			       Element chapitre = (Element)i.next();  // chapitte d'indice i
			       idChap=chapitre.getAttributeValue("id");
			       java.util.List<Element>  filsChapitre=chapitre.getChildren();
			      // System.out.println(chapitre.getAttributeValue("id"));
					Iterator i2 = filsChapitre.iterator();
					while(i2.hasNext()){
						Element nomfils= (Element)i2.next();
						String f= nomfils.getName();

						if (f=="segment"){
							idSegment=nomfils.getAttributeValue("id");
							java.util.List<Element> paragraphes = nomfils.getChildren();
							Iterator i3 = paragraphes.iterator();
							while(i3.hasNext()){
								Element paragraphe = (Element)i3.next();
								idParagraphe=paragraphe.getAttributeValue("id");
								//System.out.println(paragraphe.getText());
								Lecteur.write("<type=\"simple\" chapitre=\""+idChap+"\" segment=\""+idSegment+"\" paragraphe=\""+idParagraphe+"\" >");Lecteur.newLine();
								Lecteur.write(paragraphe.getText());Lecteur.newLine();
							}
						}
						else{
							if(f=="section"){
								idSection=nomfils.getAttributeValue("id");
								java.util.List<Element> section_fils = nomfils.getChildren();
								Iterator i3 =section_fils .iterator();
								while(i3.hasNext()){
									Element section_fil = (Element)i3.next();
									String sfilq= section_fil.getName();
									if (sfilq=="segment"){
										idSegment=section_fil.getAttributeValue("id");
										java.util.List<Element> paragraphes = section_fil.getChildren();
										Iterator i4 = paragraphes.iterator();
										while(i4.hasNext()){
											Element paragraphe = (Element)i4.next();
											idParagraphe=paragraphe.getAttributeValue("id");
											//System.out.println(paragraphe.getText());
											Lecteur.write("<type=\"simple\" chapitre=\""+idChap+"\" section=\""+idSection+"\" segment=\""+idSegment+"\" paragraphe=\""+idParagraphe+"\" >");Lecteur.newLine();
											Lecteur.write(paragraphe.getText());Lecteur.newLine(); Lecteur.newLine();
										}
									}
									else{
										idSousSection=section_fil.getAttributeValue("id");
										java.util.List<Element> segments = section_fil.getChildren();
										Iterator i5 = segments.iterator();
										while(i5.hasNext()){
											Element seg= (Element)i5.next();
											idSegment=seg.getAttributeValue("id");
											java.util.List<Element> paragraphes = seg.getChildren();
											Iterator i6 = paragraphes.iterator();
											while(i6.hasNext()){
												Element paragraphe = (Element)i6.next();
												idParagraphe=paragraphe.getAttributeValue("id");
												//System.out.println(paragraphe.getText());
												Lecteur.write("<type=\"simple\" chapitre=\""+idChap+"\" section=\""+idSection+"\" sous-section=\""+idSousSection+"\" segment=\""+idSegment+"\" paragraphe=\""+idParagraphe+"\" >");Lecteur.newLine();
												Lecteur.write(paragraphe.getText()); Lecteur.newLine()	;
											}
											
										}
										
									}
									
								}
						}
					}
			       

			    }
			    }		

			 
			 	String titre="";
			 	Iterator ii = chapitres.iterator();
				while(ii.hasNext())
			    {
				 
			       Element chapitre = (Element)ii.next();  // chapitte d'indice i
			       idChap=chapitre.getAttributeValue("id");
			       
			       Lecteur.write("<type=\"titre-chapitre\" chapitre=\""+idChap+"\">");Lecteur.newLine(); // titre chapitre
			       Lecteur.write("{S}"+ enleveEspace(chapitre.getChildText("titre")));Lecteur.newLine();
			      // System.out.println(chapitre.getChildText("titre"));
			       java.util.List<Element>  filsChapitre=chapitre.getChildren();
					Iterator i2 = filsChapitre.iterator();
					while(i2.hasNext()){
						Element nomfils= (Element)i2.next();
						String f= nomfils.getName();
						
						if (f=="segment"){
							idSegment=nomfils.getAttributeValue("id");
							
							//Lecteur.write("<type=\"titre-segment\" chapitre=\""+idChap+"\" segment=\""+idSegment+"\" >");Lecteur.newLine();
							//Lecteur.write(nomfils.getChildText("titre"));Lecteur.newLine();
							
							java.util.List<Element> paragraphes = nomfils.getChildren();
							Iterator i3 = paragraphes.iterator();
							while(i3.hasNext()){
								Element paragraphe = (Element)i3.next();
								idParagraphe=paragraphe.getAttributeValue("id");
								//System.out.println(paragraphe.getText());
								//Lecteur.write("<type=\"simple\" chapitre=\""+idChap+"\" segment=\""+idSegment+"\" paragraphe\""+idParagraphe+"\" >");Lecteur.newLine();
								//Lecteur.write(paragraphe.getText());Lecteur.newLine();
							}
						}
						else{
							if(f=="section"){
								idSection=nomfils.getAttributeValue("id");
								
								Lecteur.write("<type=\"titre_section\" chapitre=\""+idChap+"\" section=\""+idSection+"\" >");Lecteur.newLine();
								Lecteur.write("{S}"+  enleveEspace(nomfils.getChildText("titre")));Lecteur.newLine();
								
								java.util.List<Element> section_fils = nomfils.getChildren();
								Iterator i3 =section_fils .iterator();
								while(i3.hasNext()){
									Element section_fil = (Element)i3.next();
									String sfilq= section_fil.getName();
									if (sfilq=="segment"){
										idSegment=section_fil.getAttributeValue("id");
										
										//Lecteur.write("type=\"titre-segment\" <chapitre=\""+idChap+"\" section=\""+idSection+"\" segment=\""+idSegment+"\" >");Lecteur.newLine();
										//Lecteur.write(section_fil.getChildText("titre"));Lecteur.newLine();
										
										java.util.List<Element> paragraphes = section_fil.getChildren();
										Iterator i4 = paragraphes.iterator();
										while(i4.hasNext()){
											Element paragraphe = (Element)i4.next();
											idParagraphe=paragraphe.getAttributeValue("id");
											//System.out.println(paragraphe.getText());
											//Lecteur.write("type=\"simple\" <chapitre=\""+idChap+"\" section=\""+idSection+"\" segment=\""+idSegment+"\" paragraphe\""+idParagraphe+"\" >");Lecteur.newLine();
											//Lecteur.write(paragraphe.getText());Lecteur.newLine(); Lecteur.newLine();
										}
									}
									else{
										idSousSection=section_fil.getAttributeValue("id");
										//System.out.println("id ss="+idSousSection);
										
										//Lecteur.write("<type=\"titre-sous-souction\" chapitre=\""+idChap+"\" -section=\""+idSection+"\" sous-section=\""+idSousSection+"\" >");Lecteur.newLine();
										if (idSousSection!=null){
											//Lecteur.write("{S}"+section_fil.getChildText("titre"));Lecteur.newLine();
											Lecteur.write("<type=\"titre-sous-souction\" chapitre=\""+idChap+"\" section=\""+idSection+"\" sous-section=\""+idSousSection+"\" >");Lecteur.newLine();
											Lecteur.write("{S}"+ enleveEspace(section_fil.getChildText("titre")));Lecteur.newLine();
										}
										
										java.util.List<Element> segments = section_fil.getChildren();
										Iterator i5 = segments.iterator();
										while(i5.hasNext()){
											Element seg= (Element)i5.next();
											idSegment=seg.getAttributeValue("id");
											
											//Lecteur.write("<type=\"titre-segment\" chapitre=\""+idChap+"\" -section=\""+idSection+"\" sous-section=\""+idSousSection+"\" segment=\""+idSegment+"\" >");Lecteur.newLine();
											//Lecteur.write(seg.getChildText("titre"));Lecteur.newLine();
											
											java.util.List<Element> paragraphes = seg.getChildren();
											Iterator i6 = paragraphes.iterator();
											while(i6.hasNext()){
												Element paragraphe = (Element)i6.next();
												idParagraphe=paragraphe.getAttributeValue("id");
												//System.out.println(paragraphe.getText());
												//Lecteur.write("<type=\"simple\" chapitre=\""+idChap+"\" -section=\""+idSection+"\" sous-section=\""+idSousSection+"\" segment=\""+idSegment+"\" paragraphe\""+idParagraphe+"\" >");Lecteur.newLine();
												//Lecteur.write(paragraphe.getText()); Lecteur.newLine()	;
											}
											
										}
										
									}
									

								}
						}
					}

			    }
			    }
				Lecteur.close();
			 
			 
			 
			 
			
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		 
		
		 
		 
		Element e= new Element("paragraphe");
	}
	
	public String enleveEspace(String t){
	String res="";
	 Pattern p= Pattern.compile("[.](([\t]+)|([ ]+))");
	 Matcher m= p.matcher("nnd.	sdsf");
	 String[] s=p.split(t);
	 if (m.find()){
		// System.out.println(m.group(0));
		 //System.out.println("\""+s.length);
		 for(int i=0;i<s.length-1;i++){
			 res=res+s[i]+"- ";
		 }
		 res=res+s[s.length-1];
	 }

	return res;
		
	}
	
	/*public static void main(String Args[]){
		//System.out.println(2+4);
		ConvertionSengmentToText C=new ConvertionSengmentToText("/home/etudiant/Bureau/ProjetM1Sujet1/Corpustest.xml");
		C.ConvertionText();
	}*/
	
	

}
