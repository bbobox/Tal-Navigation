package extraction;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import fr.umlv.unitex.jni.UnitexJni;
/**
 * Class Principale de la Partie Extraction :
 * 	qui regroupe toute la parties qui permettent la Construction de fichier XML de representation du document
 * @author BOKA Yao
 */
public class ConstructionXmlDoc {
	private String nomdocument;
	private String nomBrut;
	
	
	
	
	/**
	 * @param doc: nom du document  traiter (fichier.txt)
	 */
	public ConstructionXmlDoc(String doc){
		nomdocument=doc;
		nomBrut=doc;
		Pattern p= Pattern.compile("(.*)[\".][a-zA-Z]*");
		Matcher m= p.matcher(nomdocument);
		if (m.find()){
			nomdocument=(m.group(1));
		}
		//System.loadLibrary("UnitexJni");
		
		
	}
	
	public void sortieXML(){
		//System.load("src/libUnitexJni.so");
		
		File file = new File(nomdocument+"_snt");
		file.mkdir();
		
		File filexml = new File(nomdocument+"_xml");
		filexml.mkdir();
		
		String persistentAlphabet = UnitexJni.loadPersistentAlphabet("French/Alphabet.txt");
		
		/* Partie Preprocessing */
			/* 1- Application de la normalisation sur le document : Normalize  */
		UnitexJni.execUnitexTool( "UnitexTool Normalize "+nomBrut+" -rFrench/Norm.txt \"--output_offsets="+nomdocument+"_snt/normalize.out.offsets\" -qutf8-no-bom");
		
			/* 2- Application Sequençage du document */
				/*-Complilation de la Grammaire */
		UnitexJni.execUnitexTool( "UnitexTool Grf2Fst2 \"French/Graphs/Preprocessing/Sentence/Sentence.grf\" -y \"-aFrench/Alphabet.txt\" -qutf8-no-bom");
				/* Transformation en transducteur à etats finis: Flatten */
		UnitexJni.execUnitexTool("UnitexTool Flatten \"French/Graphs/Preprocessing/Sentence/Sentence.fst2\" --rtn \"-d5\" -qutf8-no-bom");
				/* Application du Transducteur au texte en phase de pretraitement : Fst2Text*/
		UnitexJni.execUnitexTool("UnitexTool Fst2Txt \"-t"+nomdocument+".snt\" French/Graphs/Preprocessing/Sentence/Sentence.fst2 \"-aFrench/Alphabet.txt\" -M \"--input_offsets="+nomdocument+"_snt/normalize.out.offsets\" --output_offsets="+nomdocument+"_snt/normalize.out.offsets\" -qutf8-no-bom"); 
		UnitexJni.execUnitexTool( "UnitexTool Grf2Fst2 \"French/Graphs/Preprocessing/Replace/Replace.grf\" -y \"-aFrench/Alphabet.txt\" -qutf8-no-bom");
		UnitexJni.execUnitexTool("UnitexTool Fst2Txt \"-t"+nomdocument+".snt\" French/Graphs/Preprocessing/Replace/Replace.fst2 \"-aFrench/Alphabet.txt\" -R \"--input_offsets="+nomdocument+"_snt/normalize.out.offsets\" --output_offsets="+nomdocument+"_snt/normalize.out.offsets\" -qutf8-no-bom"); 
		
		/* Ce programme découpe le texte en unités lexicales : Tokenize */
		
		UnitexJni.execUnitexTool("UnitexTool Tokenize \""+nomdocument+".snt\" -aFrench/Alphabet.txt \"--input_offsets="+nomdocument+"_snt/normalize.out.offsets\" --output_offsets="+nomdocument+"_snt/tokenize.out.offsets\" -qutf8-no-bom");
		
		
		/* Application de Dictionnaire au Texte ( deja decoupé par la fonction Tokenize) :*/
	    UnitexJni.execUnitexTool("UnitexTool Dico \"-t"+nomdocument+".snt\" -aFrench/Alphabet.txt \"French/Dela/dela-fr-public.bin\" French/Dela/ajouts80jours.bin \"French/Dela/motsGramf-.bin\" -qutf8-no-bom");
	    
	    /* Tri Lexicographiqye des  lignes du fichier Fichier S: SortTxt */
	    
	    UnitexJni.execUnitexTool("UnitexTool SortTxt \""+nomdocument+"_snt/dlf\" -l"+nomdocument+"_snt/dlf.n \"-oFrench/Alphabet_sort.txt\" -qutf8-no-bom");
	    
	    UnitexJni.execUnitexTool("UnitexTool SortTxt \""+nomdocument+"_snt/dlc\" -l"+nomdocument+"_snt/dlc.n \"-oFrench/Alphabet_sort.txt\" -qutf8-no-bom");
	    
	    UnitexJni.execUnitexTool("UnitexTool SortTxt \""+nomdocument+"_snt/err\" -l"+nomdocument+"_snt/err.n \"-oFrench/Alphabet_sort.txt\" -qutf8-no-bom");
	    
	    UnitexJni.execUnitexTool("UnitexTool SortTxt \""+nomdocument+"_snt/tags_err\" -l"+nomdocument+"_snt/tags_err.n \"-oFrench/Alphabet_sort.txt\" -qutf8-no-bom");
	    
	    
	    
	    /* Partie Recherche de Relation Semantique dans le Document */
			    
		UnitexJni.execUnitexTool("UnitexTool Cassys \"-aFrench/Alphabet.txt\" -lFrench/CasSys/cass.csc \"-t"+nomdocument+".snt\" -v \"-rFrench/Graphs/\" --input_offsets="+nomdocument+"_snt/normalize.out.offsets\" -qutf8-no-bom");
		
		//production de la concordance */
		
		UnitexJni.execUnitexTool("UnitexTool Concord \""+nomdocument+"_snt/concord.ind\" -l40 -r55 \"--CL\" --merge=resText.SNT \"-aFrench/Alphabet.txt");
		
		
		/*--------------------------------------------------------------------------------------------------*/
		
		
		System.out.println("--- Fin de la Partite UNITEX");
		System.out.println("--- Copie du fichier snt UNITEX");
		
		try {
			BufferedWriter script1= new BufferedWriter(new FileWriter("testscript.sh"));
			
			script1.write("#!/bin/bash"); script1.newLine(); script1.newLine();
			script1.write("iconv "+nomdocument+".snt -f utf16 -t utf8  > "+nomdocument+"Conv.txt"); script1.newLine(); script1.newLine();
			script1.write("exit 0");
			script1.close();
			
			
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		 String[] cmdText2={"./testscript.sh"};
		 try {
			Process procConv=Runtime.getRuntime().exec(cmdText2);
			try {
				procConv.waitFor();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} catch (IOException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}
		 

		
		 System.out.println("--- Fin de Copie du fichier SNT UNITEX------------");
		 
		 System.out.println("--- Recherche d'ENTITE NOMME AVEC TAGEN------------");
		
		//ETAPE 2: Utilisation de TagEN pour la reconnaissance des entite Nommées 
		 
		 File ftag = new File(nomdocument+"Conv.tag.txt");
		 if (ftag.exists()){
			 ftag.delete();
		 }
		 
		 
		 try {
				BufferedWriter script2= new BufferedWriter(new FileWriter("scriptagEN.sh"));
				
				script2.write("#!/bin/bash"); script2.newLine(); script2.newLine();
				script2.write("cd TagEN ; ./tagen :mucfr ../"+nomdocument+"Conv.txt"); script2.newLine(); script2.newLine();
				script2.write("exit 0");
				script2.close();
				
				
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		 
		
		 String[] cmd= {"./scriptagEN.sh"};
		 try {
			 Process proc=Runtime.getRuntime().exec(cmd);
			 try {
				proc.waitFor();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		 } catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		 }
		 
		 System.out.println("--- fin de recherche  d'ENTITE NOMME AVEC TAGEN------------");
		 
		 
		 // Utilasation de TreeTagger Pour L'etiquetage Morpho Synthaxique 
		 
		
		 System.out.println("--- Script Tree Tagger------------");
		 
		 try {
				BufferedWriter script3= new BufferedWriter(new FileWriter("ttgscript.sh"));
				
				script3.write("#!/bin/bash"); script3.newLine(); script3.newLine();
				script3.write("cd treetagger ; sh cmd/tree-tagger-french ../"+nomdocument+".txt > ../"+nomdocument+".ttg ;"); script3.newLine(); script3.newLine();
				script3.write("exit 0");
				script3.close();
				
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		 
		 String[] cmdTtg= { "./ttgscript.sh"};
		 try {
			Process procttg=Runtime.getRuntime().exec(cmdTtg);
			procttg.waitFor();
		} catch (IOException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		 System.out.println("--- Fin Script TreeTagger------------");
		 

		 
		 // Extration des Termes avec YateA*****************************************************************************************/
		 
		 System.out.println("--- Script YateA------------");
		 
		 try {
				BufferedWriter script4= new BufferedWriter(new FileWriter("yateaScript.sh"));
				
				script4.write("#!/bin/bash"); script4.newLine(); script4.newLine();
				script4.write("yatea -rcfile /usr/local/etc/yatea/yatea.rc "+nomdocument+".ttg ; iconv yatea.dtd > "+nomdocument+"/default/xml/yatea.dtd ;"); script4.newLine(); script4.newLine();
				script4.write("exit 0");
				script4.close();
				
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		 
		 
		 
		 String[] cmdYateA={"./yateaScript.sh"};
		 try {
			Process proc=Runtime.getRuntime().exec(cmdYateA);	
			try {
				proc.waitFor();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
		 } catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();} 
		 

		 
		 // Construction du XML des Termes
		 System.out.println("--- Copie des Concordances------------");
		 
		 
		 TestJDom T= new TestJDom(nomdocument+"/default/xml/candidates.xml",nomdocument+"TermesYatea.txt"); 
		 T.listeTermes();
		 
		 try {
				BufferedWriter script5= new BufferedWriter(new FileWriter("concordScript.sh"));
				
				script5.write("#!/bin/bash"); script5.newLine(); script5.newLine();
				script5.write("cd "+nomdocument+"_snt ; iconv concord.ind -t utf8 -f utf16 > ../"+nomdocument+"_concord.txt"); script5.newLine(); script5.newLine();
				script5.write("exit 0");
				script5.close();
				
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		 
		 String[] cmdConcordance={"./concordScript.sh"};
		 try {
			 Process procConvert=Runtime.getRuntime().exec(cmdConcordance); 
			procConvert.waitFor();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		 
		 
		 System.out.println("--- Listage des Termes------------");
		 ExtractionTermes eT= new ExtractionTermes(nomdocument+"TermesYatea.txt",nomdocument+"_concord.txt",nomdocument+"CompletTermes.txt");
		 eT.productionFichier();
		 
		 System.out.println("---  COnstruction du XML des TERMES------------");

		 IndexationsTermes ITermes= new IndexationsTermes(nomdocument+"CompletTermes.txtAlpha.txt",nomdocument+"Conv.txt");
		 ITermes.indexTermes();
		 ITermes.xmlTermes(nomdocument+"_xml/"+nomdocument+"_XmlTermes.xml");
		 
		 System.out.println("---  fin de  COnstruction du XML des TERMES------------");
		 
		 // Construction du XML des Phrases 
		 
		 XmlSequence doc= new XmlSequence(nomdocument+"_xml/"+nomdocument+"_XmlPhrases.Xml",nomdocument+"Conv.txt");
		try {
			doc.conctuctionXml();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		 
		
		 // Construction du XML des Relations
		 try {
				BufferedWriter script6= new BufferedWriter(new FileWriter("scriptCascade.sh"));
				
				script6.write("#!/bin/bash"); script6.newLine(); script6.newLine();
				script6.write("iconv "+nomdocument+"_csc.txt -f utf16 -t utf8 > "+nomdocument+"_Cascade.txt"); script6.newLine(); script6.newLine();
				script6.write("exit 0");
				script6.close();
				
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		
		String[] cmdIdentConcord={"./scriptCascade.sh"};
		 try {
			Process procCSC=Runtime.getRuntime().exec(cmdIdentConcord);
			procCSC.waitFor();
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		 
		 System.out.println("---  Construction Relation Semanqtique------------"); 
		
		IndexationRSemantiques rS= new IndexationRSemantiques(nomdocument+"_concord.txt",ITermes.getEnsembleTermes(),nomdocument+"_Cascade.txt");
		rS.xmlRSmenatiques(nomdocument+"_xml/"+nomdocument+"_XmlRelationsSemantiques.xml"); 
		
		 System.out.println("---  Fin Construction Relation Semantiques------------"); 
		
		
		// Construction du XMl des Entite Nommées 
		 
		 System.out.println("---  Construction Entite Nommées------------"); 
		
		ConstructionListeEntite fichierListeEntite= new ConstructionListeEntite(nomdocument+"Conv.tag.txt",nomdocument+"ListeEntite.txt");
		
		 
		
		IndexationsEntiteN IEntiteN= new IndexationsEntiteN(nomdocument+"ListeEntite.txt",nomdocument+"_Cascade.txt",nomdocument+"CompletTermes.txtAlpha.txt",nomdocument+"Conv.tag.txt", ITermes.getEnsembleTermes());
		IEntiteN.IndexationsEntiteN();
		IEntiteN.XmlENtiteN(nomdocument+"_xml/"+nomdocument+"_XmlEntiteN.xml");
		
		
		// Construction du Xml general du fichier 
		
		 System.out.println("--- Fin de construction Entité------------"); 
		 
		 System.out.println("---  Construction XML DU DOCUMENT------------"); 
		 
		XmlDoc d= new XmlDoc(nomdocument+"_xml/"+nomdocument+".xml",nomdocument+"_xml/"+nomdocument+"_XmlPhrases.Xml",
				nomdocument+"_xml/"+nomdocument+"_XmlTermes.xml",nomdocument+"_xml/"+nomdocument+"_XmlEntiteN.xml",
				nomdocument+"_xml/"+nomdocument+"_XmlRelationsSemantiques.xml");

	}
	
	static void xmlSegmente(String fXml,String ftxt){
		ConvertionSengmentToText C=new ConvertionSengmentToText(fXml,ftxt);
		C.ConvertionText();
		
	}
	
		
	public static void main(String[] args){
		//System.out.println(System.getProperty("java.library.path"));
		
		System.out.println("Saisir 1 pour un fichier texte et une autre touche pour utiliser un corpus segmenté au format XMl ");
	    Scanner saisieOption= new Scanner(System.in);
	    String choix= saisieOption.next();
	    String a=new String("1");
		if (choix.equals(a)){
			System.out.println("Saisir le nom du fichier Texte ( Ex : fichier.txt) ");
		    Scanner saisie1= new Scanner(System.in);
		    String nondoc1= saisie1.next();
		    ConstructionXmlDoc A=new ConstructionXmlDoc(nondoc1);
			A.sortieXML();
		}
		else{
			System.out.println("Saisir le nom du fichier Xml representant le Corpus segmenté ( Ex : fichier.txt) ");
			Scanner saisie2= new Scanner(System.in);
		    String nondoc2= saisie2.next();
		    
		    Pattern p= Pattern.compile("(.*)xml");
			Matcher m= p.matcher(nondoc2);
			String ftxt="";
			if (m.find()){
				ftxt=(m.group(1))+"txt";
			}
		    
			xmlSegmente(nondoc2,ftxt);
			ConstructionXmlDoc A=new ConstructionXmlDoc(ftxt);
			A.sortieXML(); 
			
		}
	    
	}
	

}
