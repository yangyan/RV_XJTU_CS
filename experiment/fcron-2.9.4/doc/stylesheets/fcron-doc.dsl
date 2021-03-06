<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY % html "IGNORE">
<![%html;[
<!ENTITY % print "IGNORE">
<!ENTITY docbook.dsl SYSTEM "/usr/share/sgml/docbook/stylesheet/dsssl/modular/html/docbook.dsl" CDATA dsssl>
]]>
<!ENTITY % print "INCLUDE">
<![%print;[
<!ENTITY docbook.dsl SYSTEM "/usr/share/sgml/docbook/stylesheet/dsssl/modular/print/docbook.dsl" CDATA dsssl>
]]>


<!ENTITY % decl SYSTEM "../fcron-doc.mod">
%decl;

]>

<style-sheet>

<style-specification id="print" use="docbook">
<style-specification-body> 

;; ==============================
;; customize the print stylesheet
;; ==============================

(declare-characteristic preserve-sdata?
  ;; this is necessary because right now jadetex does not understand
  ;; symbolic entities, whereas things work well with numeric entities.
  "UNREGISTERED::James Clark//Characteristic::preserve-sdata?"
  #f)

(define %generate-article-toc%
  ;; Should a Table of Contents be produced for Articles?
  #t)

(define (toc-depth nd)
  2)

(define %generate-article-titlepage-on-separate-page%
  ;; Should the article title page be on a separate page?
  #f)

(define %section-autolabel%
  ;; Are sections enumerated?
  #t)

(define %footnote-ulinks%
  ;; Generate footnotes for ULinks?
  #f)

(define %bop-footnotes%
  ;; Make "bottom-of-page" footnotes?
  #f)

(define %body-start-indent%
  ;; Default indent of body text
  0pi)

(define %para-indent-firstpara%
  ;; First line start-indent for the first paragraph
  0pt)

(define %para-indent%
  ;; First line start-indent for paragraphs (other than the first)
  0pt)

(define %block-start-indent%
  ;; Extra start-indent for block-elements
  0pt)

(define formal-object-float
  ;; Do formal objects float?
  #t)

(define %hyphenation%
  ;; Allow automatic hyphenation?
  #t)

(define %admon-graphics%
  ;; Use graphics in admonitions?
  #f)

</style-specification-body>
</style-specification>


<!--
;; ===================================================
;; customize the html stylesheet; borrowed from Cygnus
;; at http://sourceware.cygnus.com/ (cygnus-both.dsl)
;; ===================================================
-->

<style-specification id="html" use="docbook">
<style-specification-body> 

(declare-characteristic preserve-sdata?
  ;; this is necessary because right now jadetex does not understand
  ;; symbolic entities, whereas things work well with numeric entities.
  "UNREGISTERED::James Clark//Characteristic::preserve-sdata?"
  #f)

(define %generate-legalnotice-link%
  ;; put the legal notice in a separate file
  #t)

(define %funcsynopsis-decoration%
  ;; make funcsynopsis look pretty
  #t)

(define %html-ext%
  ;; when producing HTML files, use this extension
  ".html")
(define use-output-dir
  #t)
(define %output-dir%
  "HTML")
(define %root-filename%
  ;; The filename of the root HTML document (e.g, "index").
  "index")
(define %use-id-as-filename%
  ;; Use ID attributes as name for component HTML files?
  #t)
(define (chunk-skip-first-element-list)
  ;; forces the Table of Contents on separate page
  '())
(define (chunk? #!optional (nd (current-node)))
  ;; 1. The (sgml-root-element) is always a chunk.
  ;; 2. If nochunks is #t or the dbhtml PI on the root element 
  ;;    specifies chunk='no', then the root element is the only
  ;;    chunk.
  ;; 3. Otherwise, elements in the chunk-element-list are chunks
  ;;    unless they're combined with their parent.
  ;; 4. Except for bibliographys, which are only chunks if they
  ;;    occur in book or article.
  ;; 5. Except for sect1 if they are in an appendix.
  ;; 6. And except for sections, which are only chunks if they
  ;;    are not too deep
  ;;
  (let* ((notchunk (or (and (equal? (gi nd) (normalize "bibliography"))
			    (not (or (equal? (gi (parent nd)) (normalize "book"))
				     (equal? (gi (parent nd)) (normalize "article")))))
		       (and (equal? (gi nd) (normalize "section"))
			    (equal? (gi (parent nd)) (normalize "section"))
			    (>= (section-element-depth nd)
				(chunk-section-depth)))
		       (and (equal? (gi nd) (normalize "sect1"))
			    (equal? (gi (parent nd)) (normalize "appendix")))))
	 (maybechunk (not notchunk)))
    (if (node-list=? nd (sgml-root-element))
	#t
	(if (or nochunks
		(equal? (dbhtml-value (sgml-root-element) "chunk") "no"))
	    #f
	    (if (member (gi nd) (chunk-element-list))
		(if (combined-chunk? nd)
		    #f
		    maybechunk)
		#f)))))

(define (list-element-list)
  ;; fixes bug in Table of Contents generation
  '())

(define %shade-verbatim%
  ;; verbatim sections will be shaded if t(rue)
  #t)
(define (shade-verbatim-attr-element element)
  (list
   (list "BORDER"  "0")
   (list "BGCOLOR" (car (cdr (shade-verbatim-element-colors element))))
   (list "WIDTH" ($table-width$))))

(define %graphic-extensions%
  ;; graphic extensions allowed
  '("gif" "png" "jpg" "jpeg" "tif" "tiff" "eps" "epsf" ))
(define %graphic-default-extension% 
  "gif")

(define %section-autolabel%
  ;; For enumerated sections (1.1, 1.1.1, 1.2, etc.)
  #t)
(define (toc-depth nd)
  ;; more depth (2 levels) to toc; instead of flat hierarchy
  ;; 2)
  4)
(define %generate-book-toc%
  ;; Should a Table of Contents be produced for books?
  #t)
(define %generate-book-titlepage%
  ;; produce a title page for books
  #t)
(define (book-titlepage-recto-elements)
  ;; elements on a book's titlepage
  ;; note: added revhistory to the default list
  (list (normalize "title")
        (normalize "subtitle")
        (normalize "graphic")
        (normalize "mediaobject")
        (normalize "corpauthor")
        (normalize "authorgroup")
        (normalize "author")
        (normalize "editor")
        (normalize "copyright")
        (normalize "revhistory")
        (normalize "abstract")
        (normalize "pubdate")
        (normalize "releaseinfo")
        (normalize "legalnotice")))
(define %generate-article-toc% 
  ;; Should a Table of Contents be produced for articles?
  #t)
(define %generate-part-toc%
  ;; Should a Table of Contents be produced for parts?
  #t)



(define %gentext-nav-use-tables%
  #t)
(define %gentext-nav-use-ff%
  #f)
(define (default-header-nav-tbl-noff elemnode prev next prevsib nextsib)
  (let* ((book (ancestor-member (current-node) (list (normalize "book"))))
	 (info (select-elements (children book) (normalize "bookinfo")))
	 (notice (select-elements (children info) (normalize "legalnotice"))))
    (let* ((r1? (nav-banner? elemnode))
	   (r1-sosofo (make element gi: "TR"
			    (make element gi: "TH"
				  attributes: (list
					       (list "COLSPAN" "3")
					       (list "ALIGN" "center"))
				  (nav-banner elemnode))))
	   (r2? (or (not (node-list-empty? prev))
		    (not (node-list-empty? next))
		    (nav-context? elemnode)))
	   (r2-sosofo (make element gi: "TR"
			    (make element gi: "TD"
				  attributes: (list
					       (list "WIDTH" "10%")
					       (list "ALIGN" "left")
					       (list "VALIGN" "bottom"))
				  (if (node-list-empty? prev)
				      (make entity-ref name: "nbsp")
				      (make element gi: "A"
					    attributes: (list
							 (list "HREF" 
							       (href-to 
								prev))
							 (list "ACCESSKEY"
							       "P"))
					    (gentext-nav-prev prev))))
			    (make element gi: "TD"
				  attributes: (list
					       (list "WIDTH" "80%")
					       (list "ALIGN" "center")
					       (list "VALIGN" "bottom"))
				  (nav-context elemnode))
			    (make element gi: "TD"
				  attributes: (list
					       (list "WIDTH" "10%")
					       (list "ALIGN" "right")
					       (list "VALIGN" "bottom"))
				  (if (node-list-empty? next)
				      (make entity-ref name: "nbsp")
				      (make element gi: "A"
					    attributes: (list
							 (list "HREF" 
							       (href-to
								next))
							 (list "ACCESSKEY"
							       "N"))
					    (gentext-nav-next next))))))
	   (r3-sosofo (make element gi: "TR"
			    (make element gi: "TD"
				  attributes: (list
					       (list "WIDTH" "40%")
					       (list "ALIGN" "left")
					       (list "VALIGN" "top"))
				  (if (node-list-empty? notice)
				      (literal "Copyright")
				      (make element gi: "A"
					    attributes: (list
							 (list "HREF"
							       ($legalnotice-link-file$
								(node-list-first notice))))
					    (literal "Copyright")))
				  (literal " ")
				  (make entity-ref name: "copy")
				  (literal " &copyrightdate; ")
				  (make element gi: "A"
					attributes: (list
						     (list "HREF" "mailto:&emailadr;"))
					(literal "Thibault Godouet")))
			    (make element gi: "TD"
				  attributes: (list
					       (list "WIDTH" "20%")
					       (list "ALIGN" "center")
					       (list "VALIGN" "top"))
				  (literal "Fcron &version; <![%devrelease; [ (dev release)]]>"))
			    (make element gi: "TD"
				  attributes: (list
					       (list "WIDTH" "40%")
					       (list "ALIGN" "right")
					       (list "VALIGN" "top"))
				  (literal "Web page : ")
				  (make element gi: "A"
					attributes: (list
						     (list "HREF" "&webpageadr;"))
					(literal "&webpageadr;"))))))
      (if (or r1? r2?)
	  (make element gi: "DIV"
		attributes: '(("CLASS" "NAVHEADER"))
		(make element gi: "TABLE"
		      attributes: (list
				   (list "SUMMARY" "Header navigation table")
				   (list "WIDTH" %gentext-nav-tblwidth%)
				   (list "BORDER" "0")
				   (list "CELLPADDING" "0")
				   (list "CELLSPACING" "0"))
		      (if r1? r3-sosofo (empty-sosofo)))
		(make empty-element gi: "HR"
		      attributes: (list
				   (list "ALIGN" "LEFT")
				   (list "WIDTH" %gentext-nav-tblwidth%)))
		(make element gi: "TABLE"
		      attributes: (list
				   (list "SUMMARY" "Header navigation table")
				   (list "WIDTH" %gentext-nav-tblwidth%)
				   (list "BORDER" "0")
				   (list "CELLPADDING" "0")
				   (list "CELLSPACING" "0"))
		      (if r1? r1-sosofo (empty-sosofo))
		      (if r2? r2-sosofo (empty-sosofo)))
		(make empty-element gi: "HR"
		      attributes: (list
				   (list "ALIGN" "LEFT")
				   (list "WIDTH" %gentext-nav-tblwidth%))))
	  (empty-sosofo)))))


(element (varlistentry term)
  (make sequence
    (make element gi: "B"
	  (process-children-trim))
    (if (not (last-sibling?))
	(literal ", ")
	(literal ""))))

(element emphasis
  ;; make role=strong equate to bold for emphasis tag
  (if (equal? (attribute-string "role") "strong")
     (make element gi: "STRONG" (process-children))
     (make element gi: "EM" (process-children))))

(element question
  (let* ((chlist   (children (current-node)))
	 (firstch  (node-list-first chlist))
	 (restch   (node-list-rest chlist)))
    (make element gi: "DIV"
	  attributes: (list (list "CLASS" (gi)))
	  (make element gi: "P"
		(make element gi: "A"
		      attributes: (list (list "NAME" (element-id)))
		      (empty-sosofo))
		(make element gi: "H3"
		      (literal (question-answer-label (current-node)) " ")
		(process-node-list (children firstch))))
	  (process-node-list restch))))

</style-specification-body>
</style-specification>

<external-specification id="docbook" document="docbook.dsl">

</style-sheet>

