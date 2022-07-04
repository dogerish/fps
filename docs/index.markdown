---
layout: page
title: Progress
---

{% for update in site.data.updates %}
<div markdown="1" class="progress-report">
### {{ update.caption }} <span class="progress-date">{{ update.timestamp | 
date: "%Y-%m-%d %H:%M" }}</span>

{% if update.image %}
![{{ update.image.alt }}]({{ "/assets/" | append: update.image.src | 
relative_url }})
{% endif %}

{% if update.video_id %}
<video controls preload="none">
	<source src="{{ "/assets/" | append: update.video_id | append: ".mp4" | 
	relative_url }}" type="video/mp4">
</video>
{% endif %}

{% if update.youtube_id %}
<iframe width="560" height="315" src="https://www.youtube.com/embed/{{ 
update.youtube_id }}" title="YouTube video player" frameborder="0" 
allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; 
picture-in-picture" allowfullscreen></iframe>
{% endif %}

</div>
{% endfor %}
